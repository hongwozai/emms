<!DOCTYPE html>
<html>
<head>
<admintpl file="header"/>
<style>
.home_info li em {
float: left;
width: 100px;
font-style: normal;
}
li {
list-style: none;
}

h4.well {
min-height: 20px;
padding: 19px;
margin-bottom: 20px;
background-color: #ecf0f1;
border: 1px solid #d7e0e2;
-webkit-border-radius: 3px;
-moz-border-radius: 3px;
border-radius: 3px;
-webkit-box-shadow: inset 0 1px 1px rgba(0,0,0,0.05);
-moz-box-shadow: inset 0 1px 1px rgba(0,0,0,0.05);
box-shadow: inset 0 1px 1px rgba(0,0,0,0.05);
}
</style>
<script type="text/javascript">
function getCpu() {
    var node = document.getElementById("cpu");
	var xhr;
	if(window.ActiveXObject){
			xhr = new ActiveXObject("Microsoft.XMLHTTP");
		}else{
			xhr = new XMLHttpRequest();
		}
    	xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                node.innerHTML = xhr.responseText;
            } else {
            	node.innerHTML = xhr.responseText;
                //alert("socket error!\n");
            }
        }
    }
    xhr.open('GET', '../socket.php?cpu=C', true);
    xhr.send();
} 
function getMem() {
    var node = document.getElementById('mem');
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                node.innerHTML = xhr.responseText;
            } else {
                alert("socket error!\n");
            }
        }
    }
    xhr.open('GET', '../socket.php?mem=M', true);
    xhr.send();
}

function getDisk() {
    var node = document.getElementById('home_devteam');
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (xhr.readyState == 4) {
            if (xhr.status == 200) {
                node.innerHTML = xhr.responseText;
            } else {
                alert("socket error!\n");
            }
        }
    }
    xhr.open('GET', '../socket.php?disk=H', true);
    xhr.send();
}

function getSysInfo(){
	getCpu();
	getMem();
	getDisk();
    setInterval(getCpu,1000);
    setInterval(getDisk,3000);
    setInterval(getMem,2000);
}

</script>
</head>
<body onload="getSysInfo()">

<div class="wrap">  
	<h4 class="well">CPU状态</h4>
      <div id="cpu" class="home_info">
      
      </div>
      <h4 class="well">内存状态</h4>
      <div id="mem" class="home_info">
      
      </div>
      <h4 class="well">硬盘状态</h4>
      <div class="home_info" id="home_devteam">
      
      </div>
   </div>
</body>
</html>
