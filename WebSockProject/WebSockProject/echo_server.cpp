#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <iostream>

using namespace std;

map<int, vector<int>> readShp();

int** initMap(map<int, vector<int>> pointsource);

vector<vector<int>> getConnectivityValue(int** map, int start);


typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;


map<int, vector<int>> mapPointSource;
int** mapValue;
// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;
    
    //getConnectivityValue(MapValue, atoi(msg->get_payload().c_str()));

    string value = msg->get_payload().c_str();
   // cout << value;
    vector<vector<int>> outPut = getConnectivityValue(mapValue, atoi(value.c_str()));
    string sendMes = "";
    for (int i = 0; i < outPut.size();i++) {
        sendMes += "road:";
        for (int j = 0; j < outPut[i].size(); j++) {
            sendMes += to_string(outPut[i][j]) + ",";
        }
    }

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        return;
    }

    try {
        //s->send(hdl, msg->get_payload(), msg->get_opcode());
        s->send(hdl, sendMes, msg->get_opcode());
    } catch (websocketpp::exception const & e) {
        std::cout << "Echo failed because: "
                  << "(" << e.what() << ")" << std::endl;
    }
}

int main() {

    mapPointSource = readShp();
    mapValue =  initMap(mapPointSource);

    //getConnectivityValue(MapValue, 13);

    server echo_server;

    try {
        // Set logging settings
        echo_server.set_access_channels(websocketpp::log::alevel::all);
        echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        echo_server.init_asio();

        // Register our message handler
        echo_server.set_message_handler(bind(&on_message,&echo_server,::_1,::_2));

        // Listen on port 9002
        echo_server.listen(9002);

        // Start the server accept loop
        echo_server.start_accept();

        // Start the ASIO io_service run loop
        echo_server.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}
