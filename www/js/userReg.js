
function checkRegForm(theForm)
{
  var InvalidateForm=true;
  var disp_info="";

  if(theForm.username.value == '')
  {
	  alert('Please input user name.');
	  return false;	
  }
  else if(theForm.userPwd.value=='')
  {
    alert('Please input user password.');
    return false;
  }
  else if(theForm.userPwd.value.length > 20 || theForm.userPwd.value.length < 6)
  {
     alert('Password length is 6~20.');
	 return false;
  }
  else if(SpecialWordRegCheck(theForm.userPwd) == true)
  {  
	 alert("Password has invlid character.");
	 return false;
  }
  else if(CnWordRegCheck(theForm.userPwd) == true)
  {
	 alert("Password has chinese character.");
	 return;
  }
  else if(theForm.userNick.value == '')
  {
	 alert("Please input nick name.");
	 return false;
  }

	return true;
}



//check new user
function checkNewUser(appRunUrl, username)
{
	if(username == '')
	{
		alert('Please input user name.');
		return;
	}

	var url = appRunUrl+"/union/checkNewUser/"+username;

	window.open(url, 'newWin', 'width=300, height=300, status=no,resizable=non,scrollbars=no');
}


