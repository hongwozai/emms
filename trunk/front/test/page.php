<html>
<head>
<style type="text/css">
.table-bordered {
border: 1px solid #ddd;
border-collapse: separate;
border-left: 0;
-webkit-border-radius: 3px;
-moz-border-radius: 3px;
border-radius: 3px;
}
.table {
width: 60%;
margin-bottom: 20px;
}
table {
max-width: 60%;
background-color: transparent;
border-collapse: collapse;
border-spacing: 0;
}
</style>
</head>
<body>
<?php
//$t1 = xdebug_time_index();
 $conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl');
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
 $sql_exc_page = OCIParse($conn,"select * from MONITOR_KEY order by MONITOR_ID desc");
 OCIExecute($sql_exc_page);
 $toltalnum = oci_fetch_all($sql_exc_page,$result);
 $totalpage = ceil($toltalnum/$perNumber);
 $sql_fenye = "select * from MONITOR_KEY order by MONITOR_ID desc";
 $sql_exc_fenye = oci_parse($conn, $sql_fenye);
 oci_execute($sql_exc_fenye);
 $mID = 0;
 
 for($i = 0; $i <= $perNumber * ($page - 1); $i++)
 {
     if( ( $row = oci_fetch_assoc($sql_exc_fenye) ) != false)
     {
         $mID = $row['MONITOR_ID'];
     }
     else
     {
         print "<script>alert('无记录')</script>";
     }
 }
 $sql_fenye = "select * from MONITOR_KEY  where MONITOR_ID <= $mID  order by MONITOR_ID desc";
 $sql_exc = oci_parse($conn, $sql_fenye);
 oci_execute($sql_exc);
  
 
 echo "<table class='table table-bordered'>";
 echo "<tr>\n";
 echo " <td width='15%' align='center' bgcolor='#f1f1f1'><strong>类型</strong></td>";
 echo "<td width='32%' align='center' bgcolor='#f1f1f1'><strong>关键字</strong></td>";
 echo "<td width='32%' align='center' bgcolor='#f1f1f1'><strong>操作</strong></td>";
 echo "</tr>";
 
 for ($i = 0; $i < $perNumber; $i++)
 {
     if(($row = oci_fetch_assoc($sql_exc)) != false)
     {
         echo "<tr>";
 
         echo "<td align='center'>".$row['TYPE']."</td>";
         echo "<td align='center'>".$row['KEYWORD']."</td>";
         $thisID = $row['MONITOR_ID'];
         ?>
 			<td align="center"><a href="sipblack_delete.php?deleteid=<?php echo $thisID;?>">删除</a></td>
 		 
 		</tr>
 	
 		<?php
 		}
 		   
 	}
 
 	echo "</table>";
 
 ?>
 </tr>
       </table>
       <li align="right">
<a href="../test/page.php?page=<?php echo 1;?>">首页</a>
<?php 
if($page != 1)		
{
?>
<a href="../test/page.php?page=<?php echo $page - 1;?>">上页 </a>
<?php  
}
if ($page < $totalpage )
{
?>
<a href="../test/page.php?page=<?php echo $page +1; ?>" >下页</a>
<?php	
}
?>
<a href="../test/page.php?page=<?php echo $totalpage;?>" >尾页 </a>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</li> <?php 
 /*
//总记录数
$sql = "select ROWNUM rn from T_USER";
$par = oci_parse($conn, $sql);
oci_execute($par);
$nRecords = ocifetchstatement($par, $rs);


//取得页码
$page = $_GET['page'];
//定义每页显示信息条数
$page_size = 3;
//当页码参数为空时，将页码设为1
if ($page == "")
{
    $page = 1;
}
//当页码大于1时，每页的开始记录是 (页码-1) * 每页记录数 +1
$startRow = ($page - 1) * $page_size + 1;
$endRow = $page * $page_size;

//方法一：
$cmdstr = "select *
from
(
    select ROWNUM rn ,temp.*
    from (select * from T_USER) temp
    where ROWNUM <= $endRow
)
where rn >= $startRow";

$parsed = ociparse($conn, $cmdstr);
oci_define_by_name($parsed, "USERNAME", $username);
oci_define_by_name($parsed, "PASSWORD", $psw);
oci_define_by_name($parsed, "PRIORITY", $pri);
oci_define_by_name($parsed, "VERIFY", $ver);
ociexecute($parsed);
$nrows = ocifetchstatement($parsed, $results);
//表字段名获取
$arrName = array_keys($results);?>
<table class="table" style="border:1px solid #ddd;" align="center">
<caption class="text-center"><h4>用户信息表</h4></caption>
<tr>
<th style="border:1px solid #ddd;" align="center" bgcolor="#ddd">用户名</th>
<th style="border:1px solid #ddd;" align="center" bgcolor="#ddd">申请权限</th>
<th style="border:1px solid #ddd;" align="center" bgcolor="#ddd">操作</th>
</tr>
<?php 
//循环输出记录
for ($i = 0; $i < $nrows; $i++)
{
    echo "<tr>\n";
    foreach ($results as $data)
    {
        echo "<td style=\"border:1px solid #ddd;\" align=\"center\">$data[$i]</td>\n";
    }
    echo "</tr>\n";
}
echo "<tr><td colspan='" . count($arrName) . "'> Number of Rows:".$nRecords."</td></tr></table>\n<br>";
//显示分页
//Pages: First Prev   1 2 3 4 5 6> Next Last
//总页数
$totalPage = ceil($nRecords / $page_size);
//上一页链接
$Prev = $page - 1;
if ($Prev < 1)
{
    $Prev = 1;
}
//下一页链接
$Next = $page + 1;
if ($Next > $totalPage)
{
    $Next = $totalPage;
}
//输出上一页链接
if ($page <> 1)
{
    echo '<span><a href="?page=1">First</a></span>';
    echo '<span><a href="?page=' . $Prev . '">Prev</a></span>';
}
else
{
    echo '<span>First</span>';
    echo '<span>Prev</span>';
}
//页码数字链接
//显示的数字个数
$pageNumber = 5;
//页码数算法
$pagebegin = $page - $pageNumber;
if ($page == 1)
{
    $pageend = $pageNumber;
}
else
{
    $pageend = $page + $pageNumber;
}
if ($pagebegin <= 0)
{
    $pagebegin = 1;
}
if ($pageend > $totalPage)
{
    $pageend = $totalPage;
}
//一次向前翻$pageNumber行
if ($page > $pageNumber)
{
    echo '<span><a href="?page=' . ($page - $pageNumber) . '"><<</a></span>';
}
//输出动态生成的页码链接
for ($i = $pagebegin; $i <= $pageend; $i++)
{
    if ($i == $page)
    {
        echo '<span style="background:#FFCC99">' . $i . '</span>';
    }
    else
    {
        echo '<span><a href="?page=' . $i . '">' . $i . '</a></span>';
    }
}
//一次向后翻$pageNumber行
if (($totalPage - $page) > 5)
{
    echo '<span><a href="?page=' . ($page + $pageNumber) . '">>></a></span>';
}
//输出下一页链接
if ($page <> $totalPage)
{
    echo '<span><a href="?page=' . $Next . '">Next</a></span>';
    echo '<span><a href="?page=' . $totalPage . '">Last</a></span>';
}
else
{
    echo '<span>Next</span>';
    echo '<span>Last</span>';
}
oci_close($conn);
$t2 = xdebug_time_index();
echo "<br/><br/>execute time:";
echo $t2-$t1;
//echo "s";*/
?>
</body>
</html>