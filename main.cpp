/*
 * main.cpp
 *
 *  Created on: Mar 24, 2024
 *      Author: afzal
 */

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <iostream>
#include <cstring>
#include <mariadb/conncpp.hpp>
#include <mariadb/mysql.h>


using namespace std;

//g++ main.cpp  -o main -std=c++11 -lmariadbcpp

//
//CREATE TABLE location (
//   id int AUTO_INCREMENT PRIMARY KEY,
//   imei varchar(18),
//   latitude varchar(14),
//   longitude varchar(14),
//   speed varchar(8),
//   sensor varchar(12),
//   time varchar(18));
//
//


int start_server(std::unique_ptr<sql::Connection> &conn)
{
	signed int r = -1;

	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(8080);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	listen(serverSocket, 5);
	
	while(1)
	{
	        struct sockaddr_in cli_addr;
        	socklen_t clilen = sizeof(cli_addr);
        	
		cout<<"waiting for new client ... "<<endl;
		
		int clientSocket = accept(serverSocket, (struct sockaddr *) &cli_addr, &clilen);

		char buffer[1024];
		while(1)
		{
			memset(buffer, 0, sizeof(buffer));
			r=recv(clientSocket, buffer, sizeof(buffer), 0);
			if(r <= 0)
			{
		    		cout<<"connection closed ..  "<<endl ; 
		    		break;
			}
			else if(r > 0)
			{
				cout << "Message from client: " << buffer << endl;
			    try
			    {
			        // Create a new PreparedStatement
			        std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("insert into location (imei, latitude, longitude, speed, sensor, time) values (?, ?, ?, ?, ?, ?)"));
			        // Bind values to SQL statement
			        stmnt->setString(1, "123456789");
			        stmnt->setString(2, "23.456787");
			        stmnt->setString(3, "90.44646");
			        stmnt->setString(4, "45.23");
			        stmnt->setString(5, "124587");
			        stmnt->setString(6, "124587");
			        // Execute query
			        stmnt->executeQuery();
			        cout<<"saved to location table "<<endl;
			    }
			    catch(sql::SQLException& e)
			    {
			      std::cerr << "Error inserting new task: " << e.what() << std::endl;
			      return 0;
			    }
			}
		}
		close(clientSocket);
		cout<< "socket closed ..."<<endl;
	}
}



int main()
{
    try {
        sql::Driver* driver = sql::mariadb::get_driver_instance();

        sql::SQLString url("jdbc:mariadb://localhost:3306/vts_test");
        sql::Properties properties({{"user", "root"}, {"password", "winter2summer"}});
        std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));
        start_server(conn);
        conn->close();
    }
    catch(sql::SQLException& e){
        std::cerr << "Error Connecting to MariaDB Platform: " << e.what() << std::endl;
        return 1;
    }
	return 0;
}
