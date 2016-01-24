<html>
<body>
<?php 
$f=$_FILES["file"]["name"];
$ff=$_FILES['file']['tmp_name'];
$f1=substr($f,0,-4);
echo $f1;
     
if(empty($ff))
{
    echo '请选择要导入的CSV----文件！';
    //exit;
}

$handle = fopen($ff, 'r');
$result = input_csv($handle); //解析csv
fclose($handle); //关闭指针
$len_result = count($result);
if($len_result==0)
{
    echo "没有任何数据！".'<br>';
    exit;
}
include("../conn.php");
//$conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
if($conn){
   // echo "连接成功！".'<br>';
   //$date=date(YmdHis);
for($i = 1; $i < $len_result; $i++) //循环获取各字段值
{
   // $id = $result[$i][0]; 
    $data = $result[$i][0];
    $keyword =$result[$i][1];
    $sender= $result[$i][2];
    $receiver= $result[$i][3];
    $subject=$result[$i][4];
    $content=$result[$i][5];
    $attsubject=$result[$i][6];
    $attcontent=$result[$i][7];
    $protocol=$result[$i][8];
    
    $date=date('YmdHis', strtotime(str_replace('/', '-', $data)));
  // $date = to_date($data,'YYYY-MM-DD hh24:mi:ss');
    echo $date;
    $sql ="insert into $f1 (ID,DATATIME,KEYWORD,SENDER,RECEIVER,SUBJECT,CONTENT,ATTACHMENTSUBJECT,ATTACHMENTCONTENT,PROTOCOL) values($i,to_date('$date', 'YYYY-MM-DD hh24:mi:ss'),'$keyword','$sender','$receiver','$subject','$content','$attsubject','$attcontent','$protocol')";
    //$sql = substr($sql,0,-1);
  //  echo ($sql);
    
    $stmt = oci_parse($conn, $sql);    
    $e=oci_execute($stmt);
    oci_commit($conn);  
    "<br>";
    //echo $data.",";
    //echo $keyword.",";
   // echo $sender.",";
    //echo $receiver.",";
    //echo $sender.",";
    //echo $content.",";
    //echo $attsubject.",";
    //echo $attcontent.",";
    //echo $protocol."<br>";
}
echo "<script language=\"JavaScript\">\r\n";
 echo "alert(\"插入成功！\");\r\n";
 echo "history.back();\r\n";
  echo "</script>";

}
else{
    echo "连接失败";
}

  function input_csv($handle)
  {
      $out = array ();
      $n = 0;
      while ($data = fgetcsv($handle, 10000))
      {
          $num = count($data);
          for ($i = 0; $i < $num; $i++)
          {
              $out[$n][$i] = $data[$i];
          }
          $n++;
      }
      return $out;
  }
 
?>
</body>
</html>