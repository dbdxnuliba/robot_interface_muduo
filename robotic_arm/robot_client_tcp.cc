#include "robot_client_tcp.h"

RobotClient::RobotClient():robot_client_log("ROBOT_CLIENT")
{
}

RobotClient::~RobotClient()
{
    robot_client_log.Write("机械臂下使能开始");
    string_tmp = "{\"cmdName\":\"disable_robot\"}";
    cmd_ptr = string_tmp.c_str();
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    int rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
    robot_client_log.Write("机械臂下使能成功");

    //断电
    robot_client_log.Write("机械臂断电开始");
    string_tmp = "{\"cmdName\":\"power_off\"}";
    cmd_ptr = string_tmp.c_str();
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
    robot_client_log.Write("机械臂断电成功");

    close(socket_fd);
}

void RobotClient::InitializeRobot(std::string address, int port)
{
    address_ = address;
    port_ = port;
    std::stringstream io;
    std::string log;
    io << "连接到IP地址：" << address << "端口号:" << port;
    io >> log;
    robot_client_log.Write(log);
    const char *address_ptr = address.c_str();
    //创建socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::stringstream io;
        std::string log;
        io << "机械臂创建客户端端口失败" << strerror(errno) << "（错误号：" << errno << "）" << std::endl;
        io >> log;
        robot_client_log.Write(log);
        exit(-1);
    }

    memset(&addr_, 0, sizeof(addr_));
    //指定IP地址版本为IPV4
    addr_.sin_family = AF_INET;
    //设置端口
    addr_.sin_port = htons(port);
    //IP地址转换函数
    if (inet_pton(AF_INET, address_ptr, &addr_.sin_addr) <= 0) {
        std::stringstream io;
        std::string log;
        io << "机械臂客户端地址转换函数出错 " << address_ptr;
        io >> log;
        robot_client_log.Write(log);
        exit(-1);
    }

    while(1) {
        if (connect(socket_fd, (struct sockaddr *) &addr_, sizeof(addr_)) < 0) {
            robot_client_log.Write("机械臂客户端等待连接中...");
            sleep(1);
        } else {
            robot_client_log.Write("机械臂客户端连接成功！");
            break;
        }
    }


    //上电
    robot_client_log.Write("机械臂上电开始");
    string_tmp = "{\"cmdName\":\"power_on\"}";
    cmd_ptr = string_tmp.c_str();
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    int rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
    robot_client_log.Write("机械臂上电成功");

    //使能
    robot_client_log.Write("机械臂上使能开始");
    string_tmp = "{\"cmdName\":\"enable_robot\"}";
    cmd_ptr = string_tmp.c_str();
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
    robot_client_log.Write("机械臂上使能成功");
}


void RobotClient::SetAout(const int &type, const int &aoutid, const float &value) {
    std::cout << "机械臂SetAout开始" << std::endl;
    //SetAout
    string_tmp = "{\"cmdName\":\"set_analog_output\",\"type\":" + std::to_string(type) + ",\"index\":" +
                 std::to_string(aoutid) + ",\"value\":" + std::to_string(value) + "}";
    cmd_ptr = string_tmp.c_str();

    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }

    int rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
//    std::cout << "Reveived: " << buf << std::endl;
    std::cout << "机械臂SetAout成功";
}

void RobotClient::MoveJ(const std::vector<float> &joint_vector, const float &velocity) {
    std::cout << "机械臂MoveJ开始";

    string_tmp = "{\"cmdName\":\"joint_move\",\"jointPosition\":[" + std::to_string(joint_vector[0]) + "," +
                 std::to_string(joint_vector[1]) + "," + std::to_string(joint_vector[2]) + "," +
                 std::to_string(joint_vector[3]) + "," + std::to_string(joint_vector[4]) + "," +
                 std::to_string(joint_vector[5]) + "]" + ",\"speed\":" + std::to_string(velocity) + ",\"relFlag\":0}";
    cmd_ptr = string_tmp.c_str();

    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    std::cout << "send task ok" << std::endl;

    int rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
//    std::cout << "Reveived: " << buf << std::endl;

    std::cout << "receive response ok" << std::endl;



    string_tmp = "{\"cmdName\":\"wait_complete\"}";
    cmd_ptr = string_tmp.c_str();

    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }

    std::cout << "send waiting ok" << std::endl;

    rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
//    std::cout << "Reveived: " << buf << std::endl;

    std::cout << "receive response" << std::endl;
    std::cout << "机械臂MoveJ成功" << std::endl;
}

void RobotClient::MoveE(const std::vector<float> &cart_vector, const float &velocity) {
    std::cout << "机械臂MoveE开始" << std::endl;

    string_tmp = "{\"cmdName\":\"end_move\",\"endPosition\":[" + std::to_string(cart_vector[0]) + "," +
                 std::to_string(cart_vector[1]) + "," + std::to_string(cart_vector[2]) + "," +
                 std::to_string(cart_vector[3]) + "," + std::to_string(cart_vector[4]) + "," +
                 std::to_string(cart_vector[5]) + "]" + ",\"speed\":" + std::to_string(velocity) + "}";
    cmd_ptr = string_tmp.c_str();
    if (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) < 0) {
        std::cerr << "Send Msg to Robot Server Error:" << strerror(errno) << "(Errno:" << errno << ")" << std::endl;
        exit(-1);
    }
    int rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
//    std::cout << "Reveived: " << buf << std::endl;

    string_tmp = "{\"cmdName\":\"wait_complete\"}";
    cmd_ptr = string_tmp.c_str();
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
//    std::cout << "Reveived: " << buf << std::endl;
    std::cout << "机械臂MoveE成功" << std::endl;
}

void RobotClient::SetRate(const float &rate_value) {
    std::cout << "机械臂SetRate开始" << std::endl;

    //SetRate
    string_tmp = "{\"cmdName\":\"rapid_rate\",\"rate_value\":" + std::to_string(rate_value) + "}";
    cmd_ptr = string_tmp.c_str();
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    int rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
//    std::cout << "Reveived: " << buf << std::endl;
    std::cout << "机械臂SetRate结束" << std::endl;
}

void RobotClient::JogStop(const int &jogcoord, const int &axis){
    std::cout << "机械臂JogStop开始" << std::endl;

    string_tmp = "{\"cmdName\":\"set_tool_id\",\"tool_id\":0}";
    cmd_ptr = string_tmp.c_str();
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    int rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
//    std::cout << "Reveived: " << buf << std::endl;

    string_tmp = "{\"cmdName\":\"jog\",\"jog_mode\":0,\"coord_map\":" + std::to_string(jogcoord) + ",\"jnum\":" +
                 std::to_string(axis) + "}";
    std::cout << string_tmp << std::endl;
    cmd_ptr = string_tmp.c_str();
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
//    std::cout << "Reveived: " << buf << std::endl;

    std::cout << "机械臂JogStop结束" << std::endl;
}

void RobotClient::Jog(const int &jogmode, const int &jogcoord, const int &axis, const float &speed, const float &coord) {
    std::cout << "机械臂Jog开始" << std::endl;

    string_tmp = "{\"cmdName\":\"set_tool_id\",\"tool_id\":0}";
    cmd_ptr = string_tmp.c_str();
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    int rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
    //    std::cout << "Reveived: " << buf << std::endl;

    string_tmp = "{\"cmdName\":\"jog\",\"jog_mode\":2,\"coord_map\":" + std::to_string(jogcoord) + ",\"jnum\":" +
                 std::to_string(axis) + ",\"jogvel\":" + std::to_string(speed) + ",\"poscmd\":" + std::to_string(coord) + "}";
//    std::cout << string_tmp << std::endl;
    cmd_ptr = string_tmp.c_str();
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    std::cout << "send task ok" << std::endl;
    rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
    std::cout << "receive response ok" << std::endl;
//    std::cout << "Reveived: " << buf << std::endl;

    string_tmp = "{\"cmdName\":\"wait_complete\"}";
    cmd_ptr = string_tmp.c_str();
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    std::cout << "send waiting ok" << std::endl;
    rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
    std::cout << "receive response ok" << std::endl;
    //    std::cout << "Reveived: " << buf << std::endl;

    std::cout << "机械臂Jog结束" << std::endl;
}

void RobotClient::GetRobotPose(std::vector<float>&joint, std::vector<float>&cart) {
    std::cout << "机械臂GetRobotPose开始" << std::endl;
    float Arr[6], Arr2[6];
    //get data
    cmd_ptr = "{\"cmdName\":\"get_data\"}";
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    // 延时0.1s，以便于接受完所有信息
    usleep(2e5);
    int rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';
//    std::cout << "Reveived: " << buf << std::endl;

    int joint_size ;
    int cart_size;
    while(1) {
        joint_size = 0;
        cart_size = 0;
        try {
            auto root = nlohmann::json::parse(buf);
            std::vector<float> joint_actual_position = root["joint_actual_position"];
            std::vector<float> actual_position = root["actual_position"];
            joint_size = joint_actual_position.size();
            cart_size = actual_position.size();
            //解析成功，读取到的数据正确：退出循环
            if (joint_size == 6 && cart_size == 6) {
                for (size_t i = 0; i < joint_size; ++i) {
                    Arr[i] = joint_actual_position[i];
                }
                for (size_t i = 0; i < cart_size; ++i) {
                    Arr2[i] = actual_position[i];
                }
                break;
            }
                // 解析成功，但是读取到的数据不正确：重新发送命令，重新解析
            else {
                cmd_ptr = "{\"cmdName\":\"get_data\"}";
                while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
                    close(socket_fd);
                    InitializeRobot(address_, port_);
                    robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
                }
                usleep(2e5);
                int rec_len = recv(socket_fd, buf, MAXLINE, 0);
                buf[rec_len] = '\0';
//                std::cout << "Reveived: " << buf << std::endl;
            }
        }
        // 解析不成功：重新发送命令，重新解析
        catch(std::exception &e) {
            cmd_ptr = "{\"cmdName\":\"get_data\"}";
            while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
                close(socket_fd);
                InitializeRobot(address_, port_);
                robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
            }
            usleep(2e5);
            int rec_len = recv(socket_fd, buf, MAXLINE, 0);
            buf[rec_len] = '\0';
//            std::cout << "Reveived: " << buf << std::endl;
        }
    }

    joint.clear();
    joint.push_back(Arr[0]);
    joint.push_back(Arr[1]);
    joint.push_back(Arr[2]);
    joint.push_back(Arr[3]);
    joint.push_back(Arr[4]);
    joint.push_back(Arr[5]);

    cart.clear();
    cart.push_back(Arr2[0]);
    cart.push_back(Arr2[1]);
    cart.push_back(Arr2[2]);
    cart.push_back(Arr2[3]);
    cart.push_back(Arr2[4]);
    cart.push_back(Arr2[5]);

    std::cout << "机械臂GetRobotPose结束" << std::endl;
}

void RobotClient::Shutdown() {
    std::cout << "机械臂Shutdown开始" << std::endl;

    cmd_ptr = "{\"cmdName\":\"shutdown\"}";
    std::cout << "Send Command to Robot Server:" << cmd_ptr << std::endl;
    while (send(socket_fd, cmd_ptr, strlen(cmd_ptr), 0) <= 0) {
        close(socket_fd);
        InitializeRobot(address_, port_);
        robot_client_log.Write("机械臂客户端发送数据错误，尝试重新建立连接");
    }
    int rec_len = recv(socket_fd, buf, MAXLINE, 0);
    buf[rec_len] = '\0';

    std::cout << "机械臂Shutdown结束" << std::endl;

}