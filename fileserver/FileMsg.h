/*
 *  Filename:   FileMsg.h
 *  Author:     xiebaoma
 *  Date:       2025-06-21
 *  Description:文件传输协议定义
 */
#pragma once

#include <stdint.h>

enum file_msg_type
{
    file_msg_type_unknown,
    msg_type_upload_req,
    msg_type_upload_resp,
    msg_type_download_req,
    msg_type_download_resp,
};

enum file_msg_error_code
{
    file_msg_error_unknown,  // 未知错误
    file_msg_error_progress, // 文件上传或者下载进行中
    file_msg_error_complete, // 文件上传或者下载完成
    file_msg_error_not_exist // 文件不存在
};

// 客户端网络类型
enum client_net_type
{
    client_net_type_broadband, // 宽带
    client_net_type_cellular   // 移动网络
};

#pragma pack(push, 1) // 字节对齐
// 协议头
struct file_msg_header
{
    int64_t packagesize; // 指定包体的大小
};

#pragma pack(pop)
