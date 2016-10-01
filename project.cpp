#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>  
#include <unistd.h>
#include <pthread.h>
typedef long long int ll;
using namespace std;
void readFile(vector<vector<ll> > &mat1, vector<vector<ll> > &mat2);
void connectToServer(int &);
void* sendMatrixOrder(void * argument);
void *sendData(void * argument);
int clientSocket;
int main(){
	vector<vector<ll> > mat1,mat2;
	readFile(mat1,mat2);
	if(mat1[0].size()!=mat2.size()){
		cout << "Cannot multiply";
		exit(0);
	}
	pthread_t thread,thread1;
	connectToServer(clientSocket);
	pair<ll,ll> order_mat1 = make_pair(mat1.size(),mat1[0].size());
	pair<ll,ll> order_mat2 = make_pair(mat2.size(),mat2[0].size());
	pair<pair<ll,ll>, pair<ll,ll> > orders=(make_pair(order_mat1,order_mat2));
	if(pthread_create(&thread,NULL,sendMatrixOrder,&orders)){
		cout << "Failed to create thread...\n";
		exit(0);
	}
	pthread_join(thread,NULL);
	pair<vector<vector<ll> >,vector<vector<ll> > > p = make_pair(mat1,mat2);
	if(pthread_create(&thread1,NULL,sendData,&p)){
		cout << "Failed to create thread...\n";
		exit(0);
	}
	pthread_join(thread,NULL);
	int counter=0;
	cout << "Receiving Result: " << endl;
	cout << "Result " << endl;
	char Buffer[1024];
	memset(Buffer,'\0',sizeof(Buffer));
	int k=0;
	while(1){
		int rec = recv(clientSocket,&Buffer[k],sizeof(Buffer[k]),0);
		k++;
		if(rec==0)
			break;
	}
	int *intBuffer = (int *) Buffer;
	int col =0;
	for(int i =  0;i<(k/4);i++){
		cout << ntohl(intBuffer[i]) << " ";
		col = (col+1)%mat2[0].size();
		if(col==0)
			cout << endl;
	}
	/*for(int row=0;row<mat1.size();row++){
		for(int col=0;col<mat2[0].size();col++){
			int element;
			read(clientSocket, &element, sizeof(element));
			element = ntohl(element);
			
			cout << swapped << " ";
			sleep(1);
		}
		cout << endl;
	}*/
	return 0;
}
void readFile(vector<vector<ll> > &mat1, vector<vector<ll> > &mat2){
	ifstream fin;
	fin.open("matrix.csv");
	string line;
	int f = 1;
	while(getline(fin, line)){
		if(!line.empty()){
			vector<string> rowstring;
			vector<ll> row;
			boost::split(rowstring, line, boost::is_any_of(","));
			for(ll i=0;i<rowstring.size();i++){
				ll no;
				no = atoi(rowstring[i].c_str());
				row.push_back(no);
			}
			if(f==1)
				mat1.push_back(row);
			else
				mat2.push_back(row);
		}else{
			f = 2;
		}
	}
}
void connectToServer(int &clientSocket){
	struct sockaddr_in serverDetails;
	socklen_t addr_size;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serverDetails.sin_family = AF_INET;
	serverDetails.sin_port = htons(60134);
	serverDetails.sin_addr.s_addr = inet_addr("127.0.1.1");
	if(connect(clientSocket, (struct sockaddr *) &serverDetails,sizeof(serverDetails))<0){
	    cout << "Could not connect to Server!" << endl;
	    exit(0);
	}else{
		cout << "Connection established between Server and Client!" << endl;
		sleep(1);
	}
}
void* sendMatrixOrder(void * argument){
	pair<pair<ll,ll>, pair<ll,ll> > *orders_pointer = (pair<pair<ll,ll>, pair<ll,ll> > *) argument;
	pair<pair<ll,ll>, pair<ll,ll> > orders = *(orders_pointer);
	pair<ll,ll> order_mat1 = (orders.first);
	pair<ll,ll> order_mat2 = (orders.second);
	cout << "Sending order of Matrices..." << endl;;
	sleep(1);
	int i = order_mat1.first;
	cout << "Sending Row1 = " << i << endl;
	i = htonl(i);
	sleep(1);
	write(clientSocket, &i, sizeof(i));
	i = order_mat1.second;
	cout << "Sending Col1 = " << i << endl;
	sleep(1);
	i = htonl(i);
	write(clientSocket, &i, sizeof(i));
	i = order_mat2.first;
	cout << "Sending Row2 = " << i << endl;
	sleep(1);
	i = htonl(i);
	write(clientSocket, &i, sizeof(i));
	i = order_mat2.second;
	cout << "Sending Col2 = " << i << endl;
	sleep(1);
	i = htonl(i);
	write(clientSocket, &i, sizeof(i));
}
void *sendData(void * argument){
	pair<vector<vector<ll> >,vector<vector<ll> > > *p_pointer = (pair<vector<vector<ll> >,vector<vector<ll> > > *) argument;
	pair<vector<vector<ll> >,vector<vector<ll> > > p = *(p_pointer);
	vector<vector<ll> > mat1 = p.first;
	vector<vector<ll> > mat2 = p.second;
	int r1 = mat1.size();
	int c1 = mat1[0].size();
	int r2 = mat2.size();
	int c2 = mat2[0].size();
	int row=0;
	while(row<r1){
		cout << "Sending Row " << row << " of Matrix 1" << endl;
		cout << "Sending ";
		for(int i=0;i<mat1[row].size();i++){
			int element = mat1[row][i];
			cout << element << " ";
			element = htonl(element);
			write(clientSocket, &element, sizeof(element));
			sleep(1);
		}
		cout << endl;
		cout << "Sending Complete Matrix2: " << endl;
		for(int i=0;i<mat2.size();i++){
			for(int j=0;j<mat2[i].size();j++){
				int element = mat2[i][j];
				cout << element << " ";
				element = htonl(element);
				write(clientSocket, &element, sizeof(element));
				sleep(1);
			}
			cout << endl;
		}
		cout << endl;
		row++;
	}
}
