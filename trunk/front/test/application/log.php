<html>
<meta charset="UTF-8" />
<script language="javascript">
window.onload=function showtable(){
var tablename=document.getElementById("mytable");
var li=tablename.getElementsByTagName("tr");
for (var i=0;i<=li.length;i++){
if (i%2==0){
li[i].style.backgroundColor="#f1f1f1";
}else li[i].style.backgroundColor="#FFFFFF";
}
}
</script>
<style>

.nav-tabs {
border-bottom: 1px solid #ddd;
}
.table {
width: 80%;
margin-bottom: 20px;
}
table {
max-width: 80%;
background-color: transparent;
border-collapse: collapse;
border-spacing: 0;
}


.button, .button:visited {
    background: #66CDAA url(overlay.png) repeat-x;
    display: inline-block;
    padding: 5px 10px 6px;
    color: #fff;
    text-decoration: none;
    -moz-border-radius: 6px;
    -webkit-border-radius: 6px;
    -moz-box-shadow: 0 1px 3px rgba(0,0,0,0.6);
    -webkit-box-shadow: 0 1px 3px rgba(0,0,0,0.6);
    text-shadow: 0 -1px 1px rgba(0,0,0,0.25);
    border-bottom: 1px solid rgba(0,0,0,0.25);
    position: relative;
    cursor: pointer
}
</style>
<body>
<?php
error_reporting(0);
session_start();
if(isset($_SESSION['username'])){
$begin=$_POST['begin'];
$end=$_POST['end'];
$l1=$_POST['lx'];
$l2=$_GET['lx'];
if($l1==NULL) $lx=$l2;
if($l2==null) $lx=$l1;
$x1=$_POST['xy'];
$x2=$_GET['xy'];
if($x1==NULL) $xy=$x2;
if($x2==null) $xy=$x1;

//echo $begin.$end;
?>
<div >
        <ul class="nav nav-tabs">
            <h4>日志管理</h4>
        </ul>
            <form name="myform" action="../application/log.php" method="post" target="_self">
        <table align="center" valign="middle" cellspacing="0" border="0" width="60%">
        <tr>
            <th align="left">开始日期：
                <input class="input" onclick="fPopCalendar(begin,begin);" return flase type="text" name="begin" size="30"/>
            </th>
            <th align="left">结束日期:
                <input class="input" onclick="fPopCalendar(end,end);" return flase type="text" name="end" size="30"/>
            </th>
        </tr>
        <tr>
          <th align="left">类型：
          <input type="radio" name="lx" value="MONITOR_LOG"<?php if($lx =="MONITOR_LOG") echo "checked=checked;"?> />监测
          <input type="radio" name="lx" value="KILL_LOG"<?php if($lx == "KILL_LOG") echo "checked=checked;"?> />封堵
          <input type="radio" name="lx" value="SPAM_LOG"<?php if($lx == "SPAM_LOG") echo "checked=checked;"?> />垃圾邮件
          </t>
        </tr>
        <tr>
          <th align="left">协议：
          <input type="radio" name="xy" value="25" <?php if($xy == "25") echo "checked=checked;"?> /> SMTP
          <input type="radio" name="xy" value="110"<?php if($xy == "110") echo "checked=checked;"?> /> POP3
          <input type="radio" name="xy" value="143"<?php if($xy == "143") echo "checked=checked;"?> /> IMAP
          <input type="radio" name="xy" value="80"<?php if($xy == "80") echo "checked=checked;"?> /> WEBMAIL
          </th>
        </tr>
    </table>
    <br>
    <table align="center" valign="middle"  width="60%" >
        <tr>
        <th style="vertical-align:top;">
        <form name="myform" action="../application/log.php?lx=<?php echo $lx;?>&xy=<?php echo $xy;?>" method="post" target="_self">
        <button type="submit" name="submit" class="button">查询</button>
        </form></th>
        <th style="vertical-align:top;">
        <form name="form" action="../application/leadin.php" method="post" enctype="multipart/form-data">
        <button type="submit" name="submit" class="button">导入</button>
        <input type="file" name="file" >
        </form></th>
        <th>
        <form name="form1" action="../application/leadout.php?name=<?php echo $lx;?>&value=<?php echo $xy;?>" method="post" target="_self">
        <button type="submit" name="submit" class="button">导出</button>
        </form></th>
        <th style="vertical-align:top;">
        <form name="form2" action="../application/delall.php?name=<?php echo $lx;?>&value=<?php echo $xy;?>" method="post" target="_self">
        <button type="submit" name="submit" class="button">删除</button>
        </form>
        </th>
        </tr>
    </table>
        </form>
<?php
$data=date(yyyy-mm-dd);
include("../conn.php");
//$conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
if($conn){
    if($begin == NULL){

            echo "<table class=\"table table-bordered\" align=\"center\" style=\"border:1px solid #ddd;table-layout:fixed;\" valign=\"middle\" cellspacing=\"0\" id=\"mytable\" >";
            echo "<caption class=\"text-center\"><h4>$lx 日志信息表</h4></caption>";
            echo "<tr  border=\"1px\">";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">日期</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">关键字</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">发件人</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">收件人</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">主题</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">内容</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">附件名</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">附件内容</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">协议</th>";
            echo "</tr>";
        /*  $stmt = oci_parse($conn, "SELECT to_char(DATATIME, 'yyyy-mm-dd hh24:mi:ss')as DATATIME,KEYWORD,SENDER,RECEIVER,SUBJECT,CONTENT,ATTACHMENTSUBJECT,ATTACHMENTCONTENT,PROTOCOL FROM $lx where PROTOCOL='$xy' order by DATATIME desc");
         // oci_define_by_name($stmt, "ID", $id);
          oci_define_by_name($stmt, "DATATIME", $data);
          oci_define_by_name($stmt, "KEYWORD", $mkey);
          oci_define_by_name($stmt, "SENDER", $sender);
          oci_define_by_name($stmt, "RECEIVER", $receiver);
          oci_define_by_name($stmt, "SUBJECT", $subject);
          oci_define_by_name($stmt, "CONTENT", $content);
          oci_define_by_name($stmt, "ATTACHMENTSUBJECT", $attsubject);
          oci_define_by_name($stmt, "ATTACHMENTCONTENT", $attcontent);
          oci_define_by_name($stmt, "PROTOCOL", $protocol);
          oci_execute($stmt);*/
            $perNumber = 8;
            $page = 1;

            if(isset($_GET['page']))
            {
                $page = $_GET['page'];
            }
            //$page = $_GET['page'];

            if( $page == false )
            {
                $page = 1;
            }
            $sql_exc_page = OCIParse($conn,"SELECT to_char(DATATIME, 'yyyy-mm-dd hh24:mi:ss')as DATATIME,KEYWORD,SENDER,RECEIVER,SUBJECT,CONTENT,ATTACHMENTSUBJECT,ATTACHMENTCONTENT,PROTOCOL FROM $lx where PROTOCOL='$xy' order by DATATIME desc");
            OCIExecute($sql_exc_page);
            $toltalnum = oci_fetch_all($sql_exc_page,$result);
            $totalpage = ceil($toltalnum/$perNumber);
            $sql_fenye = "select * from $lx order by DATATIME desc";
            $sql_exc_fenye = oci_parse($conn, $sql_fenye);
            oci_execute($sql_exc_fenye);
            $mID = 0;

            for($i = 0; $i <= $perNumber * ($page - 1); $i++)
            {
                if( ( $row = oci_fetch_assoc($sql_exc_fenye) ) != false)
                {
                    $mID = $row['ID'];
                }
                else
                {
                    print "<script>alert('无记录')</script>";
                }
            }
            $sql_fenye = "SELECT to_char(DATATIME, 'yyyy-mm-dd hh24:mi:ss') as DATATIME,KEYWORD,SENDER,RECEIVER,SUBJECT,CONTENT,ATTACHMENTSUBJECT,ATTACHMENTCONTENT,PROTOCOL FROM $lx where PROTOCOL='$xy' and ID <='$mID' order by DATATIME desc";
            $sql_exc = oci_parse($conn, $sql_fenye);
            oci_execute($sql_exc);
            for ($i = 0; $i < $perNumber; $i++){
             if(($row = oci_fetch_assoc($sql_exc)) != false){
                //$row['SENDER']=str_replace(Chr(32)," ",$row['SENDER']);
               //ECHO $row['SENDER'];
                //echo $sender;
            echo "<tr>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['DATATIME']."</td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['KEYWORD']."</td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['SENDER']."</td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['RECEIVER']."</td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['SUBJECT']."</td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\"><div style=\"height:100px;display:block;overflow:hidden;text-overflow:ellipsis;\">".$row['CONTENT']."</div></td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['ATTACHMENTSUBJECT']."</td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\"><div style=\"height:100px;display:block;overflow:hidden;text-overflow:ellipsis;\">".$row['ATTACHMENTCONTENT']."</div></td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['PROTOCOL']."</td>";
            echo "</tr>";
             }
          }
          echo "</table>";
}else{
   // if($lx==$jc){
            echo "<table class=\"table table-bordered\" align=\"center\" style=\"border:1px solid #ddd;table-layout:fixed;\" valign=\"middle\" cellspacing=\"0\" id=\"mytable\" >";
            echo "<caption class=\"text-center\"><h4>$lx 日志信息表</h4></caption>";
            echo "<tr  border=\"1px\">";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">日期</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">关键字</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">发件人</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">收件人</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">主题</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">内容</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">附件名</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">附件内容</th>";
            echo "<th style=\"border:1px solid #ddd;height:30px;\" align=\"center\" bgcolor=\"#f1f1f1\" width=\"100 px\">协议</th>";
            echo "</tr>";
        //$stmt = oci_parse($conn, "SELECT  to_char(DATATIME, 'yyyy-mm-dd hh24:mi:ss')as DATATIME,KEYWORD,SENDER,RECEIVER,SUBJECT,CONTENT,ATTACHMENTSUBJECT,ATTACHMENTCONTENT,PROTOCOL FROM $lx where PROTOCOL='$xy' and DATATIME between to_date('$begin','yyyy-mm-dd hh24:mi:ss') and to_date('$end','yyyy-mm-dd hh24:mi:ss') order by DATATIME desc");
        /*oci_define_by_name($stmt, "DATATIME", $data);
        oci_define_by_name($stmt, "KEYWORD", $mkey);
        oci_define_by_name($stmt, "SENDER", $sender);
        oci_define_by_name($stmt, "RECEIVER", $receiver);
        oci_define_by_name($stmt, "SUBJECT", $subject);
        oci_define_by_name($stmt, "CONTENT", $content);
        oci_define_by_name($stmt, "ATTACHMENTSUBJECT", $attsubject);
        oci_define_by_name($stmt, "ATTACHMENTCONTENT", $attcontent);
        oci_define_by_name($stmt, "PROTOCOL", $protocol);
        oci_execute($stmt);
        while(oci_fetch($stmt)){*/
        $sql_exc_page = OCIParse($conn,"SELECT to_char(DATATIME, 'yyyy-mm-dd hh24:mi:ss')as DATATIME,KEYWORD,SENDER,RECEIVER,SUBJECT,CONTENT,ATTACHMENTSUBJECT,ATTACHMENTCONTENT,PROTOCOL FROM $lx where PROTOCOL='$xy' order by DATATIME desc");
        OCIExecute($sql_exc_page);
        $toltalnum = oci_fetch_all($sql_exc_page,$result);
        $totalpage = ceil($toltalnum/$perNumber);
        $sql_fenye = "select * from $lx order by ID desc";
        $sql_exc_fenye = oci_parse($conn, $sql_fenye);
        oci_execute($sql_exc_fenye);
        $mID = 0;

        for($i = 0; $i <= $perNumber * ($page - 1); $i++)
        {
            if( ( $row = oci_fetch_assoc($sql_exc_fenye) ) != false)
            {
                $mID = $row['ID'];
            }
            else
            {
                print "<script>alert('无记录')</script>";
            }
        }
        $sql_fenye = "SELECT  to_char(DATATIME, 'yyyy-mm-dd hh24:mi:ss')as DATATIME,KEYWORD,SENDER,RECEIVER,SUBJECT,CONTENT,ATTACHMENTSUBJECT,ATTACHMENTCONTENT,PROTOCOL FROM $lx where PROTOCOL='$xy' and DATATIME between to_date('$begin','yyyy-mm-dd hh24:mi:ss') and to_date('$end','yyyy-mm-dd hh24:mi:ss') order by DATATIME desc";
        $sql_exc = oci_parse($conn, $sql_fenye);
        oci_execute($sql_exc);

        for ($i = 0; $i < $perNumber; $i++){
            if(($row = oci_fetch_assoc($sql_exc)) != false){
            echo "<tr>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['DATATIME']."</td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['KEYWORD']."</td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['SENDER']."</td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['RECEIVER']."</td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['SUBJECT']."</td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\"><div style=\"height:100px;display:block;overflow:hidden;text-overflow:ellipsis;\">".$row['CONTENT']."</div></td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['ATTACHMENTSUBJECT']."</td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\"><div style=\"height:100px;display:block;overflow:hidden;text-overflow:ellipsis;\">".$row['ATTACHMENTCONTENT']."</div></td>";
            echo "<td style=\"border:1px solid #ddd;height:30px;word-break:break-all;word-wrap:break-word;\" align=\"center\" width=\"100 px\">".$row['PROTOCOL']."</td>";
            echo "</tr>";
            }
        }
        echo "</table>";
}
}?>
<li align="right">
<a href="?page=<?php echo 1;?>&lx=<?php echo $lx;?>&xy=<?php echo $xy;?>" target="_self">首页</a>
<?php
if($page != 1)
{
    ?>
        <a href="?page=<?php echo $page - 1;?>&lx=<?php echo $lx;?>&xy=<?php echo $xy;?>" target="_self">上页 </a>
        <?php
        }
        if ($page < $totalpage )
        {
        ?>
        <a href="?page=<?php echo $page +1; ?>&lx=<?php echo $lx;?>&xy=<?php echo $xy;?>" target="_self" >下页</a>
        <?php
        }
        ?>
     <a href="?page=<?php echo $totalpage;?>&lx=<?php echo $lx;?>&xy=<?php echo $xy;?>" target="_self" >尾页 </a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</li> <?php

}else{?>
  <script>
  alert("请登录！");
  top.location="../index.php";
  </script>
 <?php }
?>
</div>
</body>
</html>
