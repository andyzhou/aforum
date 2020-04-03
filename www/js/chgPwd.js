//form data check
function checkForm(theForm)
{
  if(theForm.oldpwd.value=='')
  {
	alert('Please input old password.');
	theForm.oldpwd.focus();
	return false;
  }
else if(SpecialWordRegCheck(theForm.oldpwd) == true)
  {  
	 alert("Old password has invlid character.");
	 return false;
  }
  else if(CnWordRegCheck(theForm.oldpwd) == true)
  {
	 alert("Old password has chinese character.");
	 return;
  }
  else if(theForm.pwd1.value=='')
  {
	alert('Please input new password.');
	theForm.pwd1.focus();
	return false;
  }
  else if(theForm.pwd2.value=='')
  {
	alert('Please input new password again.');
	theForm.pwd2.focus();
	return false;
  }
else if(SpecialWordRegCheck(theForm.pwd1) == true)
  {  
	 alert("New password has invlid character.");
	 return false;
  }
  else if(CnWordRegCheck(theForm.pwd1) == true)
  {
	 alert("New password has chinese character.");
	 return;
  }
  else if(theForm.pwd1.value != theForm.pwd2.value)
  {
	 alert("New passwords that inputed not same.");
	 return false;
  }

	if(confirm('Do you realy change password?'))
	{
		return true;
	}

  return false;
}

