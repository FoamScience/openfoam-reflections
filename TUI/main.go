// Disclaimer: AI was used to help in writing parts of this program
package main
import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"sort"
	"strings"

	tea "github.com/charmbracelet/bubbletea"
	"github.com/charmbracelet/lipgloss"
)

// JSON data structure
type Item struct {
	Description string   `json:"description"`
	Value       string   `json:"-"`
	Key         string   `json:"key"`
	ValueArray  []string `json:"-"`
}

func (i *Item) UnmarshalJSON(data []byte) error {
	type Alias Item
	aux := &struct {
		Value interface{} `json:"value"`
		*Alias
	}{
		Alias: (*Alias)(i),
	}
	if err := json.Unmarshal(data, &aux); err != nil {
		return err
	}

	switch v := aux.Value.(type) {
	case string:
		i.Value = v
	case []interface{}:
		for _, val := range v {
			strVal := fmt.Sprintf("%v", val)
			if strVal != "" {
				i.ValueArray = append(i.ValueArray, strVal)
			}
		}
	default:
		return fmt.Errorf("unexpected type for value field")
	}

	return nil
}

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
}

func main() {
	p := tea.NewProgram(initialModel())
	if err := p.Start(); err != nil {
		fmt.Printf("Error: %v", err)
		os.Exit(1)
	}
}

// Default entry point meodel
func initialModel() Model {
	baseURL := "http://0.0.0.0:18080/classes/mff__"
    defaultEndpoint := "baseModel"
	data, keys := fetchData(baseURL + defaultEndpoint)
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
	}
}

// Make sure keys from JSON endpoint have a fixed order
func sortKeys(keys []string) {
	sort.Slice(keys, func(i, j int) bool {
		keyI := keys[i]
		keyJ := keys[j]
		isKeyIType := strings.HasSuffix(keyI, "*Type")
		isKeyJType := strings.HasSuffix(keyJ, "*Type")
		if isKeyIType && !isKeyJType {
			return true
		} else if !isKeyIType && isKeyJType {
			return false
		} else {
			return keyI < keyJ
		}
	})
}

// Get data from JSON endpoint
func fetchData(url string) (map[string]Item, []string) {
	resp, err := http.Get(url)
	if err != nil {
		log.Fatalf("Error fetching data from %s: %v", url, err)
	}
	defer resp.Body.Close()

	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		log.Fatalf("Error reading response body: %v", err)
	}

	data := make(map[string]Item)
	err = json.Unmarshal(body, &data)
	if err != nil {
		log.Fatalf("Error unmarshalling JSON from %s: %v", url, err)
	}

	keys := make([]string, 0, len(data))
	for k := range data {
		keys = append(keys, k)
	}

	return data, keys
}

func (m Model) Init() tea.Cmd {
	return nil
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
			if len(item.ValueArray) > 0 {
				m.showMenu = true
				m.menuOptions = item.ValueArray
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
		case "c":
			if m.showMenu && m.menuSelection >= 0 && m.menuSelection < len(m.menuOptions) {
				selectedOption := m.menuOptions[m.menuSelection]
				m.selectedKey = selectedOption
				updatedModel, cmd := m.updateModelForSelectedItem(selectedOption)
				return updatedModel, cmd
			}
		}
	}

	return m, nil
}

// update Model when an option is selected
func (m Model) updateModelForSelectedItem(selectedKey string) (Model, tea.Cmd) {
	data, keys := fetchData(m.baseURL + selectedKey)
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
	}, nil
}

// Render the view
func (m Model) View() string {
	normal := lipgloss.NewStyle().Foreground(lipgloss.Color("#F0F0F0"))
	boldGreen := lipgloss.NewStyle().Foreground(lipgloss.Color("#66FF66")).Bold(true)
	bold := lipgloss.NewStyle().Foreground(lipgloss.Color("#F0F0F0")).Bold(true)
    gray := lipgloss.NewStyle().Foreground(lipgloss.Color("#666666"))
    blue := lipgloss.NewStyle().Foreground(lipgloss.Color("#66CCFF"))
    boldBlue := blue.Bold(true)
    boldRed := lipgloss.NewStyle().Foreground(lipgloss.Color("#FF6666")).Bold(true)

	var endpointURL string
	if m.selectedKey != "" {
		endpointURL = fmt.Sprintf("Using Endpoint: %s%s", boldBlue.Render(m.baseURL), boldBlue.Render(m.selectedKey))
	} else {
		endpointURL = fmt.Sprintf("Using Endpoint: %s", boldBlue.Render(m.baseURL))
	}

	s := endpointURL + "\n\n"
	for i, key := range m.keys {
		item := m.items[key]
		if i == m.cursor {
			s += "> "
		} else {
			s += "  "
		}
		if len(item.ValueArray) > 0 && i == m.cursor {
            s += fmt.Sprintf("%s - %s - %s %v", bold.Render(item.Key), gray.Render(item.Description), blue.Render("options:") , nonEmptyValues(item.ValueArray, boldRed))
			if m.showMenu {
				s += fmt.Sprintf(" (%s)", m.menuOptions[m.menuSelection])
			}
			s += "\n"
		} else {
			if item.Key == "Class TypeName" {
				s += fmt.Sprintf("%s - %s\n",
					boldGreen.Render(item.Value),
					gray.Render(item.Description))
			} else {
                if item.Value != "" {
                    s += fmt.Sprintf("%s - %s - %s %s\n", bold.Render(item.Key), gray.Render(item.Description), blue.Render("default:"), item.Value)
                } else {
                    s += fmt.Sprintf("%s - %s\n", bold.Render(item.Key), gray.Render(item.Description))
                }
			}
		}
	}
	s += "\nPress q to quit, enter to select, c to choose an option.\n"

	return normal.Render(s)
}

// Filter out empty values from JSON arrays
func nonEmptyValues(values []string, style lipgloss.Style) []string {
	var nonEmpty []string
	for _, v := range values {
		if v != "" {
			nonEmpty = append(nonEmpty, style.Render(v))
		}
	}
	return nonEmpty
}
