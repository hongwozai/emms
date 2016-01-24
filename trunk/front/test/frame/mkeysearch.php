<!DOCTYPE html>
<html>
<head>

<style>
.wrap {
padding: 20px 20px 70px;

}
.nav-tabs {
border-bottom: 1px solid #ddd;
}
.table-bordered {
border: 1px solid #ddd;
border-collapse: separate;
border-left: 0;
-webkit-border-radius: 3px;
-moz-border-radius: 3px;
border-radius: 3px;
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
#btn_wraper{
				text-align: center;
			}
.table{
				text-align: center;
			}
</style>
</head>
<body>
<div class="wrap">
		<ul class="nav-tabs">
			<h4>监测关键字列表显示</h4>
		</ul>
		
		<form name="myform" action="../application/mkeyshow.php" method="post" align="left">
		<label for="username" class="label" align="left">类型选择：</label>
		<select name="number">
		 
     <option value ="0">发件人</option>  
     <option value ="1">收件人</option>  
     <option value="2">主题</option>  
     <option value="3">内容</option> 
     <option value="4">附件名</option>
     <option value="5">附件内容</option> 

		</select>&nbsp;&nbsp;&nbsp;&nbsp;
		<button type="submit" name="submit">确定</button>
		
		</form>
		<br>
		<table class="table" style="border:1px solid #ddd;" align="center">
		<caption class="text-center"><h4>监测关键字列表显示</h4></caption>
		<tr>
		<th align="center" style="border:1px solid #ddd;" bgcolor="#f1f1f1" height="30px">关键字</th>
		<th align="center" style="border:1px solid #ddd;"  bgcolor="#f1f1f1" height="30px">操作类型</th>
		</tr>
		<tr>
		<td align="center" style="border:1px solid #ddd;" height="20px"> </td>
		<td align="center" style="border:1px solid #ddd;" height="20px"> </td>
		
		</tr>
		</table>
		
	</div>
</body>
</html>
