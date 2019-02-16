#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
using namespace std;

#define MX 1000005
struct sockaddr_in server, client;
struct hostent *client_ip;
socklen_t sz = sizeof(struct sockaddr_in);
int server_socket_fd, client_socket_fd;
string mail_from, rcpt_to, subject, client_name, data, main_str;
char msg[MX];

// a simple function to print error
void show_error(string str);

// function to send error to client and print the error also
void send_error(string err_msg);

// function to complete initial tasks
void initialize(int port);

//function to receive message from client
string get_message();

// function to handle HELO request
bool helo_request();

// function to check if the mail is in valid format
bool check_email(string email);

// function to handle MAIL FROM request
bool mail_from_request();

// function to handle RCPT TO request
bool rcpt_to_request();

// function to send data to client
bool data_transfer();

// function to handle QUIT request
bool QUIT_request();

// main function of server
int main(int argc, char *args[])
{
    bool f;
    int ln;

    if (argc < 2)
        show_error("Number of arguments less than 2.");

    initialize(atoi(args[1]));

    while (1)
    {
        client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client, &sz);
        if (client_socket_fd == -1)
            show_error("Client connection error.");


        data = "220 OK.";
        strcpy(msg, data.c_str());
        cout << "S: " << msg << endl;
        ln = send(client_socket_fd, msg, strlen(msg), 0);

    
        if (!helo_request())
            continue;
        if (!mail_from_request())
            continue;
        if (!rcpt_to_request())
            continue;
        if (!data_transfer())
            continue;
        if (!QUIT_request())
            continue;
    }

    close(server_socket_fd);

    return 0;
}

void show_error(string str)
{
    cout << str << endl;
    return;
}

void send_error(string err_msg)
{
    strcpy(msg, err_msg.c_str());
    ln = send(client_socket_fd, msg, strlen(msg), 0);
    show_error("S: " + err_msg);
}

void initialize(int port)
{
    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == -1)
        show_error("Socket creation error.");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    bzero(&server.sin_zero, 8);

    int bind_res = bind(server_socket_fd, (struct sockaddr *)&server, sz);
    if (bind_res == -1)
        show_error("Binding error.");

    int listen_res = listen(server_socket_fd, 5);
    if (listen_res == -1)
        show_error("Listening error.");

    return;
}

string get_message(){
    string info;
    int ln = recv(client_socket_fd, msg, MX, 0);
    msg[ln] = '\0';
    cout << "C: " << msg << endl;
    info = "";
    for (int i = 0; msg[i]; i++)
    {
        if (msg[i] == ' ')
            break;
        info += msg[i];
    }
    return info;
}

bool helo_request()
{
    string info = get_message();
    if (info == "HELO")
    {
        data = "250 Service ready.";
        strcpy(msg, data.c_str());
        cout << "S: " << msg << endl;
        int ln = send(client_socket_fd, msg, strlen(msg), 0);
        return true;
    }
    else if (info == "MAIL FROM" || info == "RCPT TO" || info == "DATA" || info == "QUIT")
    {
        send_error("503 Bad sequence of commands.");
    }
    else
    {
        send_error("500 Improper command.")
    }
    return false;
}

bool check_email(string email)
{
    regex regx("(^[a-z0-9._-]+@[a-z0-9.-]+\.[a-z]{2,}$)");

    return regex_match(email, regx);
}

bool mail_from_request()
{
    bool f = false;
    string info = get_message();
    if (info == "MAIL FROM")
    {
        mail_from = "";
        for (int i = 0; msg[i]; i++)
        {
            if (f)
                mail_from += msg[i];
            if (msg[i] == ':')
                f = true;
        }

        if (!check_email(mail_from))
        {
            send_error("Invalid email ID.");
            return false;
        }

        data = "250 OK.";
        strcpy(msg, data.c_str());
        cout << "S: " << msg << endl;
        ln = send(client_socket_fd, msg, strlen(msg), 0);

        return true;
    }
    else if (info == "HELO" || info == "RCPT TO" || info == "DATA" || info == "QUIT")
    {
        send_error("503 Bad sequence of commands.");
    }
    else
    {
        send_error("500 Improper command.")
    }
    return false;
}

bool rcpt_to_request()
{
    bool f = false;
    string info = get_message();
    if (info == "RCPT TO")
    {
        rcpt_to = "";
        for (int i = 0; msg[i]; i++)
        {
            if (f)
                rcpt_to += msg[i];
            if (msg[i] == ':')
                f = true;
        }

        if (!check_email(rcpt_to))
        {
            send_error("Invalid email ID.");

            return false;
        }

        data = rcpt_to + ".txt";
        FILE *File = fopen(data.c_str(), "r");
        if (File == NULL)
        {
            send_error("Mail box unavailable");
        }
        fclose(File);

        data = "250 OK.";
        strcpy(msg, data.c_str());
        cout << "S: " << msg << endl;
        ln = send(client_socket_fd, msg, strlen(msg), 0);

        return true;
    }
    else if (info == "HELO" || info == "MAIL FROM" || info == "DATA" || info == "QUIT")
    {
        send_error("503 Bad sequence of commands.");
    }
    else
    {
        send_error("500 Improper command.")
    }
    return false;
}

bool data_transfer()
{
    string info = get_message();
    if (info == "DATA")
    {
        data = "354 OK";
        strcpy(msg, data.c_str());
        cout << "S: " << msg << endl;
        ln = send(client_socket_fd, msg, strlen(msg), 0);
        ln = recv(client_socket_fd, msg, MX, 0);
        msg[ln] = '\0';

        cout << "C: " << endl;
        for (int i = 0; msg[i]; i++)
        {
            ln = 0;
            if (i < (strlen(msg) - 1))
                ln = count(msg + i, msg + i + 1, '\n');
            if (ln)
            {
                cout << "\nC: " << endl;
                continue;
            }
            cout << msg[i];
        }
        cout << endl;

        /// write in file
        data = rcpt_to + ".txt";
        FILE *File = fopen(data.c_str(), "a+");
        fprintf(File, "%s\n", msg);
        fprintf(File, "****************************************************\n\n");
        fclose(File);

        data = "250 OK";
        strcpy(msg, data.c_str());
        cout << "S: " << msg << endl;
        ln = send(client_socket_fd, msg, strlen(msg), 0);

        return true;
    }
    else if (info == "HELO" || info == "MAIL FROM" || info == "RCPT TO" || info == "QUIT")
    {
        send_error("503 Bad sequence of commands.");
    }
    else
    {
        send_error("500 Improper command.")
    }
    return false;
}

bool QUIT_request()
{
    string info = get_message();
    if (info == "QUIT")
    {
        data = "221 BYE";
        strcpy(msg, data.c_str());
        cout << "S: " << msg << endl;
        ln = send(client_socket_fd, msg, strlen(msg), 0);

        close(client_socket_fd);

        return true;
    }
    else if (info == "HELO" || info == "MAIL FROM" || info == "RCPT TO" || info == "DATA")
    {
        send_error("503 Bad sequence of commands.");
    }
    else
    {
        send_error("500 Improper command.")
    }
    return false;
}
