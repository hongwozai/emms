<html>
<meta charset="UTF-8" />
		<title>EMMS电子邮件内容监控系统</title>
		<meta http-equiv="X-UA-Compatible" content="chrome=1,IE=edge" />
		<meta name="renderer" content="webkit|ie-comp|ie-stand">
		<meta name="robots" content="noindex,nofollow">
		<link href="../css/login.css" rel="stylesheet" /> 
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
<body>
<?php 
$uname=$_POST['username'];
$psw=$_POST['password'];
$repsw=$_POST['repassword'];
$numb=$_POST['number'];

//echo $uname,$psw,$repsw.'<br>';

if($uname == ""){
    ?>
    <script type="text/javascript">
    alert("用户名不能为空");
    window.history.go(-1);
    </script>
    <?php }
    else
        if($psw == ""){?>
            
                <script type="text/javascript">
                alert("密码不能为空");
                window.history.go(-1);
                </script>
            <?php }
        else 
                if($repsw == ""){?>
            
                <script type="text/javascript">
                alert("重复密码不能为空");
                window.history.go(-1);
                </script>
            <?php }
        else
                    if($psw!=$repsw){?>
                     <script type="text/javascript">
                        alert("两次输入密码不能为空");
                        window.history.go(-1);
                        </script>
                    <?php }
      //  echo $uname;
            // $conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
             include("../conn.php");
             $stmt = oci_parse($conn, "SELECT * FROM T_USER where USERNAME='$uname'");
             oci_execute($stmt);
             if(oci_fetch($stmt)){
                    // oci_commit($conn);
                     echo "<script language=\"JavaScript\">\r\n";
                     echo "alert(\"数据库中有这个用户名！\");\r\n";
                     echo "history.back();\r\n";
                     echo "</script>";     
                  }
               else{
              $stmt1 = oci_parse($conn, "insert into T_USER (USERNAME,PASSWORD,PRIORITY,VERIFY) values ('$uname','$psw','$numb','0')");
              oci_execute($stmt1);
              if(!oci_fetch($stmt1)){
                  oci_commit($conn);?>
                 <script language="JavaScript">
                 alert("用户注册成功,返回到登陆页面！");
                 window.location.href="../index.php";
                 </script>
             <?php  }
              else {?>
                 <script language="JavaScript">
                 alert("注册失败，请重新注册！");
                 window.location.href="../reg.php";
                 </script>
             <?php  }
            }   
                    ?>
</body>
</html>