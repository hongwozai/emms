<?php
set_time_limit(0);
error_reporting(0);
//$ip = "192.168.202.136";
//$port = 4444;
//$ip = "192.168.100.40";
//$port = 8888;
include 'ip.php';
/*
if (isset($_GET['cpu']))
    $cpu = ' ';
    else {*/
        $cpu=$_GET['cpu'];
   /*}
        
if(isset($_GET['disk']))
  $disk=' ';
  else {*/
      $disk=$_GET['disk'];
 /* }
  if(isset($_GET['mem']))
      $mem=' ';
  else {*/
      $mem=$_GET['mem'];
 /* }
  if(isset($_GET['info']))
      $ctrSys=' ';
  else {*/
      $ctrSys=$_GET['info'];
      $endSys=$_GET['info1'];
  //}
/* error handle */
function error($sockfd, $line, $file) {
  echo "ERROR ".$line.",".$file.": (".
       socket_strerror(socket_last_error($sockfd)).")<br/>";
}

/* client socket */
function client_connect($ip, $port) 
{
  $sockfd = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
  if (!$sockfd) {
    error($sockfd, __LINE__, __FILE__);
    return FALSE;
  }
  $result = socket_connect($sockfd, $ip, $port);
  if (!$result) {
    error($sockfd, __LINE__, __FILE__);
    return FALSE;
  }
  return $sockfd;
}

/* start connect ... */
$sockfd = client_connect($ip, $port);
if (!$sockfd) {
  exit(0);
}
$out_cpu ="";
$out_mem="";
$out_disk="";
$out_info="";
//echo "asdf".$cpu;
if($cpu=='C'){
    //向服务器发送信息
    if(!socket_write($sockfd,"C\n",2)){
        echo("<p>The server has been closed!</p>");
        echo "socket_write()failed: reason:".socket_strerror($sockfd)."\n";
    }else{
        //接收服务器信息
        if(!$out_cpu=socket_read($sockfd,1024)){
            echo("<p>No DATA</p>");
        }else{
            echo("<p>".(floor($out_cpu*100)/100)."%"."</p>");
        }
    }
}
if($mem=='M'){
    //向服务器发送信息
    if(!socket_write($sockfd,"M\n",2)){
        echo("<p>The server has been closed!</p>");
        echo "socket_write()failed: reason:".socket_strerror($sockfd)."\n";
    }else{
        //接收服务器信息
        if(!$out_mem=socket_read($sockfd,1024)){
            echo("<p>No DATA</p>");
        }else{
            echo("<p>".(floor($out_mem*100)/100)."%"."</p>");
        }
    }
}
if($disk=='H'){
    //向服务器发送信息
    if(!socket_write($sockfd,"H\n", 2)){
        echo("<p>The server has been closed!</p>");
        echo "socket_write()failed: reason:".socket_strerror($sockfd)."\n";
    }else{
        
        //接收服务器信息
        if(!$out_disk=socket_read($sockfd,1024)){
            echo("<p>No DATA</p>");
        }else{
            echo("<p>".(floor($out_disk*100)/100)."%"."</p>");
        }
    }
}
if($ctrSys=='S')
{
    //向服务器发送信息
    if(!socket_write($sockfd,"S\n",2)){
        echo("<p>The server has been closed!</p>");
    }else{
        //接收服务器信息
        $out_info=socket_read($sockfd,1024);
        //echo $out_info;
        if($out_info=='0'){
            echo("<p>No DATA</p>");
        }else{
            if(1==$out_info){
                //<img src="/test/imag/on.jpg">
                echo("<img src='/test/imag/on.jpg'>");
                //echo("<p>系统已经开启!</p>");
            }
            else
                echo("<img src='/test/imag/off.jpg'>");
        }
    }
}
if($endSys=='E')
{
    //向服务器发送信息
    if(!socket_write($sockfd,"E\n",2)){
        echo("<p>The server has been closed!</p>");
    }else{
        //接收服务器信息
        $out_info1=socket_read($sockfd,1024);
        //echo $out_info;
        if($out_info1=='0'){
            echo("<p>No DATA</p>");
        }else{
            if(1==$out_info1){
                //<img src="/test/imag/on.jpg">
                echo("<img src='/test/imag/off.jpg'>");
                //echo("<p>系统已经开启!</p>");
            }
            else
                echo("<img src='/test/imag/on.jpg'>");
        }
    }
}
socket_close($sockfd);
?>

