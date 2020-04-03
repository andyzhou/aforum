//form data check
function ValidateLogin(theForm)
{
  if(theForm.uname.value=='')
  {
	alert('Please input user name.');
	theForm.uname.focus();
	return false;
  }
  else if(theForm.upwd.value=='')
  {
	alert('Please input password.');
	theForm.upwd.focus();
	return false;
  }

  return true;
}

