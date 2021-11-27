function errorMsg()
{
  alert("Your browser doesn't support the plugin.");
}
function addEngine(name,ext,cat,type)
{
  if ((typeof window.sidebar == "object") && (typeof
  window.sidebar.addSearchEngine == "function"))
  {
    window.sidebar.addSearchEngine(
      "http://www-128.ibm.com/developerworks/js/"+name+".src",
      "http://www-128.ibm.com/developerworks/i/"+name+"."+ext,
      name,
      cat );
  }
  else
  {
    errorMsg();
  }
}

