<?php
  //global $user;
 // $user=array('username'=>$_POST['username'],'password'=>$_POST['password']);
  $username=$_POST['username'];
  $password=$_POST['password'];
  include("../conn.php");
 // $conn = ocilogon('scott','123','192.168.100.34:1521/linuxorcl','AL32UTF8');
  $stmt = oci_parse($conn, "SELECT * FROM T_USER where USERNAME='$username'and PASSWORD='$password' and VERIFY='1'");
 
  oci_define_by_name($stmt, "USERNAME", $username);
  oci_define_by_name($stmt, "PASSWORD", $password);
  oci_define_by_name($stmt, "PRIORITY", $priority);
  oci_define_by_name($stmt, "VERIFY", $verify);
  oci_execute($stmt);
//  session_register(user);
  if(oci_fetch($stmt))
  {
  // echo "<script language=\"JavaScript\">alert(\"你好\");</script>"; 
   session_start();
   $_SESSION['username']=$username;
   $_SESSION['password']=$password;
   $_SESSION['priority']=$priority;
   $_SESSION['verify']=$verify;
   header("Location:../sy.php");
   //exit();
  }
  else{
      
      //echo "windows.open()\n";
      header("Location:../error.php");
  }
