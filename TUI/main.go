// Disclaimer: AI was used to help in writing parts of this program
package main

import (
	"net/http"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"sort"
	"strings"
    "regexp"
    "bytes"
    "flag"

	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"
)

// JSON data structure
type Item struct {
	Key         string `json:"key"`
	Default     string `json:"default"`
	Type        string `json:"type"`
	Min         string `json:"min,omitempty"`
	Max         string `json:"max,omitempty"`
	Description string `json:"description,omitempty"`
    RTSOptions  []string `json:"rtsOptions,omitempty"`
	SubItems    map[string]Item `json:"subItems,omitempty"`
}

// RawItem is used to unmarshal the JSON into a map without Key
type RawItem map[string]interface{} //{
//    Default     string `json:"default"`
//    Type        string `json:"type"`
//    Min         string `json:"min,omitempty"`
//    Max         string `json:"max,omitempty"`
//    Description string `json:"description,omitempty"`
//}

// Model for the Bubble Tea program
type Model struct {
	items         map[string]Item
	keys          []string
	cursor        int
	selectedKey   string
	showMenu      bool
	menuOptions   []string
	menuSelection int
	baseURL       string
    jsonObject    map[string]string
    path          []string
    indentLevel   int
}

// Define a struct to hold the JSON response
type ClassesResponse struct {
	Types []string `json:"types"`
}

func parseJSONToItems(jsonData []byte) (map[string]Item, []string, error) {
	var rawItems RawItem
	err := json.Unmarshal(jsonData, &rawItems)
	if err != nil {
		return nil, nil, err
	}

	items := make(map[string]Item)
	keys := make([]string, 0, len(rawItems))
    pattern := `"<<RTSoption>>\s*\d*\s*\(\s*(.+)\s*\)"`
    rtsOptionRegex := regexp.MustCompile(pattern)
	for key, value := range rawItems {
        item := Item{Key: key}
		switch v := value.(type) {
		case map[string]interface{}:
			if _, ok := v["default"]; ok {
				item.Default = v["default"].(string)
				item.Type = v["type"].(string)
				if min, ok := v["min"]; ok {
					item.Min = min.(string)
				}
				if max, ok := v["max"]; ok {
					item.Max = max.(string)
				}
				if description, ok := v["description"]; ok {
					item.Description = description.(string)
				}
				if strings.HasPrefix(item.Default, "\"<<RTSoption>>") {
					matches := rtsOptionRegex.FindStringSubmatch(item.Default)
					if len(matches) > 1 {
						item.RTSOptions = strings.Fields(matches[1])
					}
				}
			} else {
                subItemsBytes, err := json.Marshal(v)
                if err != nil {
		            fmt.Printf("Error: %v", err)
		            os.Exit(1)
                }
                subItems, _, err := parseJSONToItems(subItemsBytes)
				item.SubItems = subItems
				//keys = append(keys, subKeys...)
			}
		default:
			continue
		}
		items[key] = item
		keys = append(keys, key)
	}

	return items, keys, nil
}

func main() {
	host := flag.String("host", "0.0.0.0", "Host to bind the Crow server")
	port := flag.Int("port", 18080, "Port to bind the Crow server")
	flag.Parse()
	p := tea.NewProgram(initialModel(*host, *port))

	if err := p.Start(); err != nil {
		fmt.Printf("Error: %v", err)
		os.Exit(1)
	}
}

// Default entry point model
func initialModel(host string, port int) Model {
	baseURL := fmt.Sprintf("http://%s:%d/classes/", host, port)
	defaultEndpoint := "fullReflectedModel"

	data, keys, _ := fetchData(baseURL, defaultEndpoint)
    sortKeys(keys)

	return Model{
		items:         data,
		keys:          keys,
		cursor:        0,
		selectedKey:   defaultEndpoint,
		showMenu:      false,
		menuOptions:   nil,
		menuSelection: -1,
		baseURL:       baseURL,
        jsonObject:    make(map[string]string),
        path:          []string{},
        indentLevel:   0,
	}
}

// Init is needed by tea.Model
func (m Model) Init() tea.Cmd {
	return nil
}

// Make sure keys from JSON endpoint have a fixed order
func sortKeys(keys []string) {
	sort.Slice(keys, func(i, j int) bool {
		keyI := keys[i]
		keyJ := keys[j]
		isKeyIType := strings.HasSuffix(keyI, "Type")
		isKeyJType := strings.HasSuffix(keyJ, "Type")

        if strings.Contains(keyI, "_Endpoint") {
            return true
        } else if isKeyIType && !isKeyJType {
			return true
		} else if !isKeyIType && isKeyJType {
			return false
		} else {
			return keyI < keyJ
		}
	})
}

// Get data from JSON endpoint
func fetchData(baseUrl string, scope string) (map[string]Item, []string, error) {
    url := baseUrl + scope
	resp, err := http.Get(url)
	if err != nil {
		log.Fatalf("Error fetching data from %s: %v", url, err)
	}
	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		log.Fatalf("Error reading response body: %v", err)
	}
    items, keys, err := parseJSONToItems(body)
    endpointOptions := ""
    // Parse the JSON response
	var classesResponse ClassesResponse
    if (strings.HasSuffix(baseUrl, "/classes/")) {
        classesUrl := strings.Replace(baseUrl, "/classes/", "/supportedClasses", 1)
        resp, err := http.Get(classesUrl)
	    if err != nil {
	    	log.Fatalf("Error fetching data from %s: %v", url, err)
	    }
	    defer resp.Body.Close()
	    body, err := ioutil.ReadAll(resp.Body)
	    if err != nil {
	    	log.Fatalf("Error reading response body: %v", err)
	    }
	    err = json.Unmarshal(body, &classesResponse)
	    if err != nil {
	    	log.Fatalf("Failed to parse JSON response: %v", err)
	    }    
        endpointOptions = "\"<<RTSoption>> ("
        for _, e := range classesResponse.Types {
            endpointOptions += " " + e
        }
        endpointOptions += " )\""
    }
    endpoint := Item {
        Key:         "_Endpoint",
        Default:     endpointOptions,
        Type:        "Class",
        Description: "The Endpoint scope to fetch class members from.",
        RTSOptions: classesResponse.Types,
        SubItems:   make(map[string]Item),
    }
    items[endpoint.Key] = endpoint;
    exists := false
    for _, item := range keys {
        if item == endpoint.Key {
            exists = true
        }
    }
    if !exists {
        keys = append(keys, endpoint.Key)
    }
    return items, keys, err
}

// Handle view updates and keymaps
func (m Model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch msg.String() {
		case "ctrl+c", "q":
			return m, tea.Quit
		case "up":
			if m.cursor > 0 {
				m.cursor--
			}
		case "down":
			if m.cursor < len(m.keys)-1 {
				m.cursor++
			}
		case "enter":
			key := m.keys[m.cursor]
			item := m.items[key]
			if len(item.RTSOptions) > 0 {
				m.showMenu = true
				m.menuOptions = item.RTSOptions
				m.menuSelection = 0
				return m, nil
			} else {
				return m, nil
			}
		case "left", "right":
			if m.showMenu {
				if msg.String() == "left" && m.menuSelection > 0 {
					m.menuSelection--
				} else if msg.String() == "right" && m.menuSelection < len(m.menuOptions)-1 {
					m.menuSelection++
				}
			}
		case "esc":
			m.showMenu = false
		case "r":
			m.jsonObject = make(map[string]string)
			err := resetConfiguration(m.baseURL + "config/reset")
			if err != nil {
				log.Printf("Error resetting configuration: %v", err)
			}
			updatedModel, cmd := m.updateModelForSelectedItem(m.selectedKey)
			return updatedModel, cmd
		case "c":
			if m.showMenu && m.menuSelection >= 0 && m.menuSelection < len(m.menuOptions) {
				selectedOption := m.menuOptions[m.menuSelection]
				currentKey := m.keys[m.cursor]
                postKey := currentKey
				trimmedKey := currentKey
				if strings.HasSuffix(currentKey, "Type") {
					trimmedKey = currentKey[:len(currentKey)-len("Type")]
				}
				if strings.Contains(trimmedKey, "_Endpoint") {
					trimmedKey = selectedOption
				}
                if m.indentLevel > 0 && currentKey != "_Endpoint" {
                    postKey = m.path[len(m.path)-1] + "Type"
                }
				m.jsonObject[postKey] = selectedOption
				updatedModel, cmd := m.updateModelForSelectedItem(trimmedKey)
				return updatedModel, cmd
			}
		case "n": // Handle "n" key press to navigate into subitems
            m.indentLevel += 1
			key := m.keys[m.cursor]
			item := m.items[key]
			if len(item.SubItems) > 0 {
				m.path = append(m.path, key)
				m.items = item.SubItems
				m.keys = make([]string, 0, len(item.SubItems))
				for subKey := range item.SubItems {
					m.keys = append(m.keys, subKey)
				}
				sortKeys(m.keys)
				m.cursor = 0
			}
		case "b": // Handle "b" key press to navigate back
            m.indentLevel -= 1
			if len(m.path) > 0 {
				m.path = m.path[:len(m.path)-1]
				if len(m.path) == 0 {
					data, keys, _ := fetchData(m.baseURL, m.selectedKey)
					sortKeys(keys)
					m.items = data
					m.keys = keys
				} else {
					parentKey := m.path[len(m.path)-1]
					parentItem := m.items[parentKey]
					m.items = parentItem.SubItems
					m.keys = make([]string, 0, len(parentItem.SubItems))
					for subKey := range parentItem.SubItems {
						m.keys = append(m.keys, subKey)
					}
					sortKeys(m.keys)
				}
				m.cursor = 0
			}
		}
	}
	return m, nil
}

// update Model when an option is selected
func (m Model) updateModelForSelectedItem(selectedKey string) (Model, tea.Cmd) {
	// Construct the JSON payload
	jsonPayload, err := json.Marshal(m.jsonObject)
	if err != nil {
		log.Fatalf("Error marshaling JSON payload: %v", err)
	}

	// Send the JSON payload to the endpoint
	postURL := m.baseURL + "config/" + selectedKey
	resp, err := http.Post(postURL, "application/json", bytes.NewBuffer(jsonPayload))
	if err != nil {
		log.Fatalf("Error posting data to %s: %v", postURL, err)
	}
	defer resp.Body.Close()

	data, keys,_ := fetchData(m.baseURL, selectedKey)
	sortKeys(keys)

	return Model{
		items:         data,
		keys:          keys,
		cursor:        0,
		selectedKey:   selectedKey,
		showMenu:      false,
		menuOptions:   nil,
		menuSelection: -1,
		baseURL:       m.baseURL,
        jsonObject:    m.jsonObject,
        path:          m.path,
        indentLevel:   m.indentLevel,
	}, nil
}

// Render the view
func (m Model) View() string {
	blue := lipgloss.NewStyle().Foreground(lipgloss.Color("#66CCFF"))
	boldBlue := blue.Bold(true)
	normal := lipgloss.NewStyle().Foreground(lipgloss.Color("#F0F0F0"))

	var endpointURL string
	if m.selectedKey != "" {
		endpointURL = fmt.Sprintf("Using Endpoint: %s%s", boldBlue.Render(m.baseURL), boldBlue.Render(m.selectedKey))
	} else {
		endpointURL = fmt.Sprintf("Using Endpoint: %s", boldBlue.Render(m.baseURL))
	}

	s := endpointURL + "\n\n"
    rendered, _ := renderItems(m.items, m.keys, m.cursor, m.showMenu, m.menuOptions, m.menuSelection, 0, 0)
    s += rendered
    s += boldBlue.Render("\nKeymaps:")
    s += boldBlue.Render("\n\t<q>") + " to quit"
    s += boldBlue.Render("\n\t<enter>") + " to select from RTS options"
    s += boldBlue.Render("\n\t\t<c>") + " to commit to an RTS option"
    s += boldBlue.Render("\n\t<r>") + " to reset RTS configuration"
    s += boldBlue.Render("\n\t<n>") + " to jump into sub-items"
    s += boldBlue.Render("\n\t<b>") + " to jump out of sub-items"
	return normal.Render(s)
}

// Helper function to render items recursively
func renderItems(items map[string]Item, keys []string, cursor int, showMenu bool, menuOptions []string, menuSelection int, indentLevel int, currentIndex int) (string, int) {
	blue := lipgloss.NewStyle().Foreground(lipgloss.Color("#66CCFF"))
	boldGreen := lipgloss.NewStyle().Foreground(lipgloss.Color("#66FF66")).Bold(true)
	bold := lipgloss.NewStyle().Foreground(lipgloss.Color("#F0F0F0")).Bold(true)
	gray := lipgloss.NewStyle().Foreground(lipgloss.Color("#666666"))
	boldRed := lipgloss.NewStyle().Foreground(lipgloss.Color("#FF6666")).Bold(true)
	var s string
	indent := strings.Repeat("  ", indentLevel)
	for _, key := range keys {
		item := items[key]
		if currentIndex == cursor {
			s += "> "
		} else {
			s += "  "
		}
		s += indent
		if len(item.RTSOptions) > 0 && currentIndex == cursor {
			s += fmt.Sprintf("%s - %s - %s %v", bold.Render(key), gray.Render(item.Description), blue.Render("options:"), nonEmptyValues(item.RTSOptions, boldRed))
			if showMenu {
				s += fmt.Sprintf(" (%s)", menuOptions[menuSelection])
			}
			s += "\n"
		} else {
			if item.Default != "" {
				s += fmt.Sprintf("%s - %s", bold.Render(key), gray.Render(item.Description))
				if strings.HasPrefix(item.Default, "\"<<RTSoption>>") {
					s += fmt.Sprintf(" - %s %s\n", blue.Render("default:"), boldGreen.Render(item.Default))
				} else {
					s += fmt.Sprintf(" - %s %s\n", blue.Render("default:"), item.Default)
				}
			} else {
				s += fmt.Sprintf("%s - %s\n", bold.Render(key), gray.Render(item.Description))
			}
		}
		currentIndex++
		if len(item.SubItems) > 0 {
			subKeys := make([]string, 0, len(item.SubItems))
			for subKey := range item.SubItems {
				subKeys = append(subKeys, subKey)
			}
			sortKeys(subKeys)
			var subItemsStr string
			subItemsStr, currentIndex = renderItems(item.SubItems, subKeys, cursor, showMenu, menuOptions, menuSelection, indentLevel+1, currentIndex)
			s += subItemsStr
		}
		if strings.Contains(item.Key, "_Endpoint") {
			s += "\n"
		}
	}
	return s, currentIndex
}

//// Filter out empty values from JSON arrays
func nonEmptyValues(values []string, style lipgloss.Style) []string {
	var nonEmpty []string
	for _, v := range values {
		if v != "" {
			nonEmpty = append(nonEmpty, style.Render(v))
		}
	}
	return nonEmpty
}

// Send DELETE request to reset the RTS configuration
func resetConfiguration(url string) error {
	req, err := http.NewRequest(http.MethodDelete, url, nil)
	if err != nil {
		return err
	}

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return fmt.Errorf("failed to reset configuration, status code: %d", resp.StatusCode)
	}

	return nil
}
