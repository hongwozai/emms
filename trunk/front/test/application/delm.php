<html>
<body>
<?php 
//echo 1;
session_start();
if($_SESSION['priority']==2){?>
                     <script type="text/javascript">
                        alert("您没有权限操作！");
                        window.history.go(-3);
                        </script>
                    <?php }else{
$id=$_GET['postid'];
$numb=$_GET['numb'];
//echo $id;
include("../conn.php");
//$conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
$stmt = oci_parse($conn, "delete from MONITOR_KEY where MONITOR_ID='$id'");
$e=oci_execute($stmt);
oci_commit($conn);
 if($e==TRUE){

     //$ip = "192.168.100.40";
     //$port="8888";
    
     include '../ip.php';
     $str="D1".$numb."\n";
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
         }else{
          ?>
             <script type="text/javascript">
             alert("删除成功！");
             window.location.href="../application/mkeyshow.php?number=<?php echo $numb;?>";
             </script>
        <?php  
         }
     }
 }
 }                   
 ?>
 </body>
 </html>