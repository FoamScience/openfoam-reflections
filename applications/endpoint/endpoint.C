////////////////////////////////////////////////////////////////////////////////
/// Look at the README in this folder for a more info //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "crow/middlewares/cors.h"
#include "dictionary.H"
#include "reflectDictionary.H"
#include <crow.h>
#include <string>
#include <type_traits>
#include "origOpenFOAMModel.H"
#include "uiModel.H"
#include "wrapperModel.H"
#include "childWrapperModel.H"

using namespace Foam;

// Function to parse command-line arguments
void parseArgs(int argc, char* argv[], std::string& host, int& port) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--host" && i + 1 < argc) {
            host = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        }
    }
}

template<class T>
static crow::json::wvalue
reflectAndBuildClassInfo(dictionary config)
{
    Info<< config << endl;
    crow::json::wvalue json;
    // Create skeleton non-constructible config
    auto skel = Reflect::reflect<T, false>::schema(config);
    // Convert to JSON
    std::function<crow::json::wvalue(const dictionary&)> parser;
    parser = [&parser](const dictionary& dict) -> crow::json::wvalue {
        crow::json::wvalue js; 
        for (auto& e: dict.keys()) {
            if (dict.isDict(e)) {
                crow::json::wvalue sub;
                js[e] = parser(dict.subDict(e));
            } else {
                js[e] = dict.get<string>(e).replaceAll("Foam::", "").replaceAll("\n"," ");
            }
        }
        return js;
    };
    return parser(skel);
}

int
main(int argc, char* argv[])
{
    std::string host = "0.0.0.0";
    int port = 18080;
    parseArgs(argc, argv, host, port);

    crow::App<crow::CORSHandler> app;
    dictionary config;
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
      .headers("X-Custom-Header", "Upgrade-Insecure-Requests")
      .methods("POST"_method, "GET"_method)
      .origin("*");

    // Endpoint to get 
    CROW_ROUTE(app, "/")
    ([]() {
        return "<b>Docs:</b><br>Hit the base class URL with a GET,"
            " something like <code>/classes/uiModel</code><br>"
            "More supported classes can be consulted (GET) at <code>/supportedClasses</code><br>"
            "And if you want to configure the RTS reflection, POST to <code>/classes/config/yourClass</code><br>"
            "Lastly, reset the configuration with a DELETE to <code>/classes/config/reset</code>";
    });

    auto postHandle = [&config](const crow::request& req) {
        auto json_data = crow::json::load(req.body);
        Info << !json_data << endl;
        if (!json_data) {
            return crow::response(400, "Invalid JSON");
        }
        for (const auto& item : json_data) {
            std::string value(item);
            config.set(string(item.key()), value);
        }
        crow::json::wvalue response_data;
        response_data["received"] = json_data;
        return crow::response(response_data);
    };


    // Endpoint to get all classes
    CROW_ROUTE(app, "/classes/origOpenFOAMModel")
    ([&]() {
        return crow::response(reflectAndBuildClassInfo<origOpenFOAMModel>(config));
    });

    // Endpoint to run configured reflection from POST response
    CROW_ROUTE(app, "/classes/config/origOpenFOAMModel").methods("POST"_method)
    (postHandle);

    // Endpoint to get all classes
    CROW_ROUTE(app, "/classes/uiModel")
    ([&config]() {
        Info<< config << endl;
        return crow::response(reflectAndBuildClassInfo<uiModel>(config));
    });
    // Endpoint to handle POST requests for uiModel config
    CROW_ROUTE(app, "/classes/config/uiModel").methods("POST"_method)
    (postHandle);

    CROW_ROUTE(app, "/classes/regularModel%3Cvector%3E")
    ([&config]() {
        Info<< config << endl;
        return crow::response(reflectAndBuildClassInfo<wrapperModel<vector>>(config));
    });
    // Endpoint to handle POST requests for uiModel config
    CROW_ROUTE(app, "/classes/config/regularModel%3Cvector%3E").methods("POST"_method)
    (postHandle);

    // Reset config
    CROW_ROUTE(app, "/classes/config/reset").methods("DELETE"_method)
    ([&config]() {
        config = dictionary::null;
        return crow::response(200, "Stored JSON data has been reset");
    });

    // route to return supported class type names
    CROW_ROUTE(app, "/supportedClasses")
    ([]() {
        crow::json::wvalue jsonResponse;
        jsonResponse["types"] = crow::json::wvalue::list({
            uiModel::typeName,
            origOpenFOAMModel::typeName,
            regularModel<vector>::typeName,
            "childRegularModel%3Cvector%3E",
        });
        return crow::response(jsonResponse);
    });

    // Redirect all remaining traffic to "/"
    CROW_CATCHALL_ROUTE(app)
    ([](const crow::request&, crow::response& res) {
        res.redirect("/");
    });

    app.bindaddr(host).port(port).run();
    return 0;
}
