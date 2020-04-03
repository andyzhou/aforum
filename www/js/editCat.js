
function checkForm(theForm)
{
  var InvalidateForm=true;
  var disp_info="";

  if(theForm.title.value == '')
  {
	  alert('Please input category name.');
	  theForm.title.focus();
	  return false;	
  }

	if(confirm('Do you want save this category?'))
	{
		return true;
	}

	return false;
}


