<html>
<body>
<?php 
header("Content-type: text/html; charset=utf-8");
$name=$_GET['name'];
$pro=$_GET['value'];
//echo $name,$pro;

$conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
//include '../conn.php';
$stmt = oci_parse($conn, "SELECT to_char(DATATIME, 'yyyy-mm-dd hh24:mi:ss')as DATATIME,KEYWORD,SENDER,RECEIVER,SUBJECT,CONTENT,ATTACHMENTSUBJECT,ATTACHMENTCONTENT,PROTOCOL FROM $name where PROTOCOL='$pro'");
  //oci_define_by_name($stmt, "ID", $id);
  oci_define_by_name($stmt, "DATATIME", $data);
  oci_define_by_name($stmt, "KEYWORD", $key);
  oci_define_by_name($stmt, "SENDER", $sender);
  oci_define_by_name($stmt, "RECEIVER", $receiver);
  oci_define_by_name($stmt, "SUBJECT", $subject);
  oci_define_by_name($stmt, "CONTENT", $content);
  oci_define_by_name($stmt, "ATTACHMENTSUBJECT", $attsubject);
  oci_define_by_name($stmt, "ATTACHMENTCONTENT", $attcontent);
  oci_define_by_name($stmt, "PROTOCOL", $protocol);
oci_execute($stmt);
$detail = "DATATIME,KEYWORD,SENDER,RECEIVER,SUBJECT,CONTENT,ATTACHMENTSUBJECT,ATTACHMENTCONTENT,PROTOCOL\r\n";
//$detail .="\n";
while(oci_fetch($stmt)){   
    //echo $data;
        /*$data= iconv('utf-8','utf-8',$data); //中文转码
       / $key= iconv('utf-8','utf-8',$key);
        $sender = iconv('utf-8','utf-8',$sender); //中文转码
        $receiver= iconv('utf-8','utf-8',$receiver);
        $subject = iconv('utf-8','utf-8',$subject); //中文转码
        $content = iconv('utf-8','utf-8',$content);
        $attsubject = iconv('utf-8','utf-8',$attsubject); //中文转码
        $attcontent = iconv('utf-8','utf-8',$attcontent);
        $protocol = iconv('utf-8','utf-8',$protocol); //中文转码*/
       // echo $data." ";
        //echo $key." ";
        //echo $sender." ";
       // echo $receiver."　";
       // echo $subject." ";
       // echo $content." ";
       // echo $attsubject." ";
       // echo $attcontent." ";
       // echo $protocol."<br>";
        $detail .= $data.",".$key.",".$sender.",".$receiver.",".$subject.",".$content.",".$attsubject.",".$attcontent.",".$protocol."\r\n";   
       // echo $detail."<br>";
}
$filename = $name.'.txt';
ob_end_clean();
header('Content-Encoding: none');
header('Content-Type: application/octet-stream');
header('Content-Disposition: attachment; filename='.$filename);
header('Pragma: no-cache');
header('Expires: 0');
$detail = iconv('UTF-8', 'utf-8', $detail); //转化编码，否则会出现乱码
echo $detail;
exit();
?>
</body>
</html>