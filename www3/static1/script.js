let toggler = true;

function changeColor() {
    if (toggler) {
	document.getElementById("title").style.color = "#0077cc";
	toggler = !toggler;
    } else
    {
	document.getElementById("title").style.color = "";
	toggler = !toggler;
    }
}
