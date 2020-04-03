
function checkForm(theForm)
{
  var InvalidateForm=true;
  var disp_info="";

  if(theForm.title.value == '')
  {
	  alert('Please input board name.');
	  theForm.title.focus();
	  return false;	
  }
  else if(theForm.bname.value == '')
  {
	  alert('Please input board ename.');
	  theForm.bname.focus();
	  return false;	
  }
  else if(SpecialWordRegCheck(theForm.bname) == true)
  {  
	 alert("Board ename has invlid character.");
	 theForm.bname.focus();
	 return false;
  }
  else if(CnWordRegCheck(theForm.bname) == true)
  {
	 alert("Board ename has chinese character.");
	 theForm.bname.focus();
	 return false;
  }
  else if(theForm.pid.value == '')
  {
	 alert("Please select category");
	 theForm.pid.focus();
	 return false;	
  }

	if(confirm('Do you want save this board?'))
	{
		return true;
	}

	return false;
}


