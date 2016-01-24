<!DOCTYPE html>
<html>

<head>
		<meta charset="UTF-8" />
		<title>EMMS电子邮件内容监控系统</title>
		<meta http-equiv="X-UA-Compatible" content="chrome=1,IE=edge" />
		<meta name="renderer" content="webkit|ie-comp|ie-stand">
		<meta name="robots" content="noindex,nofollow">
		<link href="css/login.css" rel="stylesheet" /> 
		<style>
			#login_btn_wraper{
				text-align: center;
			}
			#login_btn_wraper .tips_success{
				color:#fff;
			}
			#login_btn_wraper .tips_error{
				color:#DFC05D;
			}
			#login_btn_wraper button:focus{outline:none;}
		</style>
	<style>
.wrap1 {
	width: 250px;
	overflow:hidden;
	margin: 180px auto 0;
	-webkit-animation: bounceIn 600ms linear;
	-moz-animation: bounceIn 600ms linear;
	-o-animation: bounceIn 600ms linear;
	animation: bounceIn 600ms linear;
}
</style>
	<script type="text/javascript">
function checkLoginForm() {
    var un = document.getElementById("username");
    var pw = document.getElementById("password");
    var rpw = document.getElementById("repassword");
    if(un.value == "" ) {
        alert("用户名不能为空");
        window.history.go(-1);
    }
    if(pw.value == ""){
   	 alert("密码不能为空");
}
    if(rpw.value == ""){
     	 alert("重复密码不能为空");
  }
    if(pw.value!=rpw.value){
    	alert("两次输入的密码不一样！");
    }
</script>
</head>
<body>

<div class="wrap1">
		
		<form method="post" name="reg" action="application/check.php" autoComplete="off" class="J_ajaxForm">
		<h1 style="color:white">新用户注册</h1>
			<div class="login">
				<ul>
					<li>
						<input class="input" id="username" name="username" type="text" placeholder="帐号名" title="帐号名" />
					</li>
					<li>
						<input class="input" id="password" type="password"  name="password" placeholder="密码" title="密码" />
					</li>
					<li>
						<input class="input" id="password" type="password"  name="repassword" placeholder="重复密码" title="重复密码" />
					</li>
				     <li>
						<select class="input" id="priority" type="text"  name="number"  title="申请权限" />
						<option value="2">普通用户</option>
						<option value="1">管理员</option>
						</select>
						
					</li>
				</ul>
				
			</div>
			<div id="login_btn_wraper">
					<button type="submit" name="submit" class="btn btn_submit J_ajax_submit_btn">提交</button>
				</div>
		</form>
	</div>
</body>
</html>