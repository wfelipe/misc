function longpoll ()
{
	var req = new XMLHttpRequest ();

	req.open ("GET", "/broadcast/sub", true);

	req.onreadystatechange = handleResponse;
	req.send (null);
}

function handleResponse ()
{
	if (req.readyState != 4 && req.readyState != 3)
		return;
	if (req.readyState == 2 && req.status != 200)
		return;
	document.getElementById ('message').innerHTML = this.responseText;
}
