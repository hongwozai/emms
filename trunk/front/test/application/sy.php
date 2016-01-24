<html>
<body>
<?php 
//$logout=$_GET['logout'];
//if($logout=='out')
  
    session_start();
   //echo $_GET['logout'];
    
    //注销登录
    if($_GET['logout'] == "out"){
        unset($_SESSION['password']);
        unset($_SESSION['username']);
        unset($_SESSION['priority']);
        unset($_SESSION['vserify']);
     ?>
     <script language="JavaScript">
       alert("注销成功！");                 
     </script>  
        
  <?php  header("Location:../index.php");}
?>
</body>
</html>