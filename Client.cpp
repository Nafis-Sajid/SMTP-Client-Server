#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
using namespace std;

#define MX 1000005
struct sockaddr_in client;
struct hostent *host_ip;
int client_socket_fd, connect_res, ln;
socklen_t sz = sizeof(struct sockaddr);
string sender, receiver, host, subject, file_name, data, main_str;
char msg[MX];
int port;

// a simple function to print error
void show_error(string str);

// function to process the passed arguments
void process_args(int cnt, char *str[]);

// function to complete initial tasks
void initialize();

// function to handle HELO request
void helo_request();

// function to handle MAIL FROM request
void mail_from_request();

// function to handle RCPT TO request
void rcpt_to_request();

// function to get system date and convert into string
string get_date();

// function to get data from server
void data_transfer();

// function to handle QUIT request
voquit();

// main function of client
int main(int argc, char *args[])
{
    if (argc < 4)
        show_error("Number of arguments less than 4.");

    process_args(argc, args);
    initialize();

    connect_res = connect(client_socket_fd, (struct sockaddr *)&client, sz);
    if (connect_res == -1)
        show_error("Server connection error.");
    ln = recv(client_socket_fd, msg, MX, 0);
    msg[ln] = '\0';
    if (msg[0] != '2')
        show_error("S: Error 404");
    cout << "S: " << msg << endl;

    helo_request();
    mail_from_request();
    rcpt_to_request();
    data_transfer();
    quit();
    return 0;
}

//implementation of all thefunctions:
void show_error(string err_msg)
{
    cout << err_msg << endl;
    exit(0);
}

void process_args(int cnt, char *str[])
{
    int flag = 0;
    bool f = false;
    receiver = "";
    host = "";
    subject = "";
    file_name = "";
    port = 0;
    for (int j = 0; str[1][j]; j++)
    {
        if (str[1][j] == '@')
        {
            flag++;
            continue;
        }
        else if (str[1][j] == ':')
        {
            flag++;
            continue;
        }
        else if (flag == 0)
            receiver += str[1][j];
        else if (flag == 1)
            host += str[1][j];
        else if (flag == 2)
            port = (port * 10) + (str[1][j] - '0');
    }
    file_name = (string)str[cnt - 1];
    for (int k = 2; k < (cnt - 1); k++)
        subject += (string)str[k];
    host_ip = gethostbyname(host.c_str());
    return;
}

void initialize()
{
    client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket_fd == -1)
        show_error("Socket creating error.");

    client.sin_family = AF_INET;
    bcopy((char *)host_ip->h_addr, (char *)&client.sin_addr.s_addr, host_ip->h_length);
    client.sin_port = htons(port);
    bzero(&client.sin_zero, 8);

    return;
}

void helo_request()
{
    cout << "C: ";
    getline(cin, data);
    strcpy(msg, data.c_str());
    ln = send(client_socket_fd, msg, strlen(msg), 0);
    ln = recv(client_socket_fd, msg, MX, 0);
    msg[ln] = '\0';
    if (msg[0] != '2')
    {
        data = "S: " + (string)msg;
        show_error(data);
    }
    cout << "S: " << msg << endl;

    return;
}

void mail_from_request()
{
    cout << "C: ";
    getline(cin, data);

    bool f = false;
    sender = "";
    for (int i = 0; data[i]; i++)
    {
        if (data[i] == ':')
        {
            f = true;
            continue;
        }
        else if (f)
            sender += data[i];
    }

    strcpy(msg, data.c_str());
    ln = send(client_socket_fd, msg, strlen(msg), 0);
    ln = recv(client_socket_fd, msg, MX, 0);
    msg[ln] = '\0';
    if (msg[0] != '2')
    {
        data = "S: " + (string)msg;
        show_error(data);
    }
    cout << "S: " << msg << endl;

    return;
}

void rcpt_to_request()
{
    cout << "C: ";
    getline(cin, data);

    bool f = false;
    receiver = "";
    for (int i = 0; data[i]; i++)
    {
        if (data[i] == ':')
        {
            f = true;
            continue;
        }
        else if (f)
            receiver += data[i];
    }

    strcpy(msg, data.c_str());
    ln = send(client_socket_fd, msg, strlen(msg), 0);
    ln = recv(client_socket_fd, msg, MX, 0);
    msg[ln] = '\0';
    if (msg[0] != '2')
    {
        data = "S: ";
        data += (string)msg;
        show_error(data);
    }
    cout << "S: " << msg << endl;

    return;
}

string get_date()
{
    time_t now = time(0);

    string date = ctime(&now);

    string res[10], ans;
    int sp = 0;

    res[0] = "";
    for (int i = 0; date[i]; i++)
    {
        if (date[i] == ' ')
        {
            sp++;
            res[sp] = "";
            continue;
        }
        res[sp] += date[i];
    }

    ans = res[4] + ", " + res[0] + ", " + res[3] + "-" + res[1] + "-" + res[5];

    return ans;
}

void data_transfer()
{
    cout << "C: ";
    getline(cin, data);
    strcpy(msg, data.c_str());
    ln = send(client_socket_fd, msg, strlen(msg), 0);
    ln = recv(client_socket_fd, msg, MX, 0);
    msg[ln] = '\0';
    if (!(msg[0] == '3' && msg[1] == '5' && msg[2] == '4'))
    {
        data = "S: ";
        data += (string)msg;
        show_error(data);
    }
    cout << "S: " << msg << endl;

    main_str = "MAIL FROM: " + sender + "\n";
    cout << "C: MAIL FROM: " << sender << endl;

    main_str += "RCPT TO: " + receiver + "\n";
    cout << "C: RCPT TO: " << receiver << endl;

    main_str += "SUBJECT: " + subject + "\n";
    cout << "C: SUBJECT: " << subject << endl;

    string date = get_date();
    main_str += "DATE: " + date + "\n";
    cout << "C: DATE: " << date << endl;

    // Read from mail file
    FILE *File = fopen(file_name.c_str(), "r");
    data = "";
    while (fgets(msg, MX, File))
    {
        cout << "C: " << msg << endl;
        data = (string)msg;
        main_str += data;
    }
    fclose(File);

    strcpy(msg, main_str.c_str());
    ln = send(client_socket_fd, msg, strlen(msg), 0);
    ln = recv(client_socket_fd, msg, MX, 0);
    msg[ln] = '\0';
    if (msg[0] != '2')
    {
        data = "S: " + (string)msg;
        show_error(data);
    }
    cout << "S: " << msg << endl;

    return;
}

void quit_request()
{
    cout << "C: ";
    getline(cin, data);
    strcpy(msg, data.c_str());
    ln = send(client_socket_fd, msg, strlen(msg), 0);
    ln = recv(client_socket_fd, msg, MX, 0);
    msg[ln] = '\0';
    if (msg[0] != '2')
    {
        data = "S: " + (string)msg;
        show_error(data);
    }
    cout << "S: " << msg << endl;

    close(client_socket_fd);
    return;
}