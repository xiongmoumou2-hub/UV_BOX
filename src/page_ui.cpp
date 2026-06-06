#include "page_ui.h"

// 配置WiFi的网页代码
const String ROOT_HTML_PAGE1 PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="content-type" content="text/html; charset=UTF-8" />
    <title>登录页面</title>
    <style>
        #content,
        .login,
        .login-card a,
        .login-card h1,
        .login-help {
            text-align: center
        }
        body,
        html {
            margin: 0;
            padding: 0;
            width: 100%;
            height: 100%;
            display: table
        }
        #content {
            font-family: 'Source Sans Pro', sans-serif;
            -webkit-background-size: cover;
            -moz-background-size: cover;
            -o-background-size: cover;
            background-size: cover;
            display: table-cell;
            vertical-align: middle
        }
        .login-card {
            padding: 40px;
            width: 274px;
            background-color: #F7F7F7;
            margin: 0 auto 10px;
            border-radius: 20px;
            box-shadow: 8px 8px 15px rgba(0, 0, 0, .3);
            overflow: hidden
        }
        .login-card h1 {
            font-weight: 400;
            font-size: 2.3em;
            color: #1383c6
        }
        .login-card h1 span {
            color: #f26721
        }
        .login-card img {
            width: 70%;
            height: 70%
        }
        .login-card input[type=submit] {
            width: 100%;
            display: block;
            margin-bottom: 10px;
            position: relative
        }
        .login-card input[type=text],
        input[type=password] {
            height: 44px;
            font-size: 16px;
            width: 100%;
            margin-bottom: 10px;
            -webkit-appearance: none;
            background: #fff;
            border: 1px solid #d9d9d9;
            border-top: 1px solid silver;
            padding: 0 8px;
            box-sizing: border-box;
            -moz-box-sizing: border-box
        }
        .login-card input[type=text]:hover,
        input[type=password]:hover {
            border: 1px solid #b9b9b9;
            border-top: 1px solid #a0a0a0;
            -moz-box-shadow: inset 0 1px 2px rgba(0, 0, 0, .1);
            -webkit-box-shadow: inset 0 1px 2px rgba(0, 0, 0, .1);
            box-shadow: inset 0 1px 2px rgba(0, 0, 0, .1)
        }
        .login {
            font-size: 14px;
            font-family: Arial, sans-serif;
            font-weight: 700;
            height: 36px;
            padding: 0 8px
        }
        .login-submit {
            -webkit-appearance: none;
            -moz-appearance: none;
            appearance: none;
            border: 0;
            color: #fff;
            text-shadow: 0 1px rgba(0, 0, 0, .1);
            background-color: #4d90fe
        }
        .login-submit:disabled {
            opacity: .6
        }
        .login-submit:hover {
            border: 0;
            text-shadow: 0 1px rgba(0, 0, 0, .3);
            background-color: #357ae8
        }
        .login-card a {
            text-decoration: none;
            color: #666;
            font-weight: 400;
            display: inline-block;
            opacity: .6;
            transition: opacity ease .5s
        }
        .login-card a:hover {
            opacity: 1
        }
        .login-help {
            width: 100%;
            font-size: 12px
        }
        .list {
            list-style-type: none;
            padding: 0;
            display: flex
        }
        .list__item {
            margin: 0 0 .7rem;
            padding-left: 5px;
            color: #4d90fe;
        }
        label {
            display: -webkit-box;
            display: -webkit-flex;
            display: -ms-flexbox;
            display: flex;
            -webkit-box-align: center;
            -webkit-align-items: center;
            -ms-flex-align: center;
            align-items: center;
            text-align: left;
            font-size: 14px;
        }
        input[type=checkbox] {
            -webkit-box-flex: 0;
            -webkit-flex: none;
            -ms-flex: none;
            flex: none;
            margin-right: 10px;
            float: left
        }
        .error {
            font-size: 14px;
            font-family: Arial, sans-serif;
            font-weight: 700;
            height: 25px;
            padding: 0 8px;
            padding-top: 10px;
            -webkit-appearance: none;
            -moz-appearance: none;
            appearance: none;
            border: 0;
            color: #fff;
            text-shadow: 0 1px rgba(0, 0, 0, .1);
            background-color: #ff1215
        }
        @media screen and (max-width:450px) {
            .login-card {
                width: 70% !important
            }
            .login-card img {
                width: 30%;
                height: 30%
            }
        }
    </style>
</head>
<body style="background-color: #e5e9f2">
    <div id="content">
        <form name='input' action='/configwifi' method='POST'>
            <div class="login-card">
                <h1>ESP32 UV BOX</h1>
                <form name="login_form" method="post" action="$PORTAL_ACTION$">
                    <input type="text" name="ssid" placeholder="请输入WiFi 名称" id="ssid" list="data-list" ;
                        style="border-radius: 10px">
                    <datalist id="data-list">
)rawliteral";
const String ROOT_HTML_PAGE2 PROGMEM = R"rawliteral(
                    <input type="password" name="password" placeholder="请输入WiFi 密码" id="password" ;
                        style="border-radius: 10px">
                    <input type="text" name="key" placeholder="请输入Private KEY" id="key" ;
                        style="border-radius: 10px">
                    <input type="text" name="location" placeholder="请输入Location ID" id="location" ;
                        style="border-radius: 10px">
                    <input type="text" name="uid" placeholder="请输入bilibili UID" id="uid" ;
                        style="border-radius: 10px">
                    <div class="login-help">
                        <ul class="list">
                            <li class="list__item">
                                <a target="_blank" href="https://id.qweather.com/#/login?redirect=https%3A%2F%2Fconsole.qweather.com%2F%23%2Fapps&lang=zh">和风天气</a>
                            </li>
                            <li class="list__item">
                                <a target="_blank" href="https://docs.qq.com/sheet/DTWNWWU9FeXhwSERG?tab=BB08J2">Location ID</a>
                            </li>
                            <li class="list__item">
                                <a target="_blank" href="https://space.bilibili.com/378576508">bilibili</a>
                            </li>
                            <li class="list__item">
                                <a target="_blank" href="https://oshwhub.com/levi_01">oshwhub</a>
                            </li>
                            <li class="list__item">
                                <a target="_blank" href="https://levi52.github.io/">blog</a>
                            </li>
                            
                        </ul>
                    </div>
                    <input type="submit" class="login login-submit" value="保 存" id="login" ; disabled;
                        style="border-radius: 15px">
                </form>
            </div>
        </form>
    </div>
</body>
</html>
)rawliteral";