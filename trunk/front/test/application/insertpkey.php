<html>
<body>
<?php
 $numb=$_POST['number'];
 $str=$_POST['pkey'];
 //echo $numb."<br>";
 //echo $str."<br>";
 include("../conn.php");
 //$conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
 $stmt = oci_parse($conn, "insert into KILL_KEY values(KILL_ID_seq.nextval,'$numb','$str')");
 
 $e=oci_execute($stmt);
 oci_commit($conn);
 
if($e==TRUE){

   //  $ip = "192.168.100.40";
   //  $port="8888";
    include '../ip.php';
     $str="A0".$numb."\n";
     $output=0;
     set_time_limit(0);
     function error($sockfd, $line, $file) 
     {
         echo "ERROR ".$line.",".$file.": (".
         socket_strerror(socket_last_error($sockfd)).")<br/>";
     }
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
    
     $sockfd = client_connect($ip, $port);
     if (!$sockfd) 
     {
         exit(0);
     }
     
     if(!socket_write($sockfd,$str,strlen($str)))
     {
         echo("<p>The server has been closed!</p>");
         //echo "socket_write()failed: reason:".socket_strerror($sockfd)."\n";
     }else{
         //接收服务器信息
         if(!$output=socket_read($sockfd,1024)){
             echo("<p>No DATA</p>");
         }else{?>
            <script language="JavaScript">
             alert("插入成功！");
              window.location.href="../frame/plugkey.php";
             </script>
        <?php  }
     }
}else{?>
 
            <script type="text/javascript">
            alert("插入重复！！");
            window.location.href="../frame/plugkey.php";
            </script>
                 
 <?php  }
 ?>
 </body>
 </html>