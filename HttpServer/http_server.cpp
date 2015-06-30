#include "server_http.hpp"
#include "client_http.hpp"
#include "image_search.h"
#include "image_search_controller.h"

//Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

//Added for the default_resource example
#include<fstream>

using namespace std;
//Added for the json-example:
using namespace boost::property_tree;

typedef Web::Server<Web::HTTP> HttpServer;
typedef Web::Client<Web::HTTP> HttpClient;

int main() {

	//list_index("root");
	build_index("root");

	return 0;



	//HTTP-server at port 8080 using 4 threads
	HttpServer server(8080, 1);

	// queries:
	// /build/root					--- build all
	// /build/specific-node-path	--- build node: /specific/node/path
	server.resource["^/uGuide/([a-zA-Z0-9_.-]+)$"]["GET"] = [](HttpServer::Response& response, shared_ptr<HttpServer::Request> request) {
		string path = request->path_match[1];

		try {
			run_sdk();
		}
		catch (exception & ex)
		{
			cerr << "Exception: " << ex.what() << endl;
		}

		string respond = "good";
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << respond.length() << "\r\n\r\n" << respond;
	};

	server.default_resource["GET"] = [](HttpServer::Response& response, shared_ptr<HttpServer::Request> request) {
		string content = "Invalid request";
		response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
	};

	init_sdk();

	//thread server_thread([&server](){
	//	//Start server
	//	server.start();
	//});
	server.start();

	//server_thread.join();
	end_sdk();
	return 0;
}