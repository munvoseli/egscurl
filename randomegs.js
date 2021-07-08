const cComic = comicData.length;
const elImage = document.getElementById ("img");
const elButton = document.getElementById ("button");
const elPrev = document.getElementById ("prev");
const elNext = document.getElementById ("next");
const elLink = document.getElementById ("a");
var nComic = 0;

function setComic (i)
{
    nComic = i;
    var nSpace = comicData [nComic].indexOf (" ");
    elImage.src = "https://www.egscomics.com/comics/" + comicData [nComic].substring (nSpace + 1);
    elLink.href = "https://www.egscomics.com/comic/" + comicData [nComic].substring (0, nSpace);
    elButton.innerText = nComic + 1;
}

function randomComic () {setComic (Math.floor (Math.random() * cComic))};
function nextComic () {setComic (nComic + 1)};
function prevComic () {setComic (nComic - 1)};

//elImage.addEventListener ("click", randomComic, false);
elButton.addEventListener ("click", randomComic, false);
elPrev.addEventListener ("click", prevComic, false);
elNext.addEventListener ("click", nextComic, false);
//window.addEventListener ("click", randomComic, false);
addEventListener ("keydown", function (e) {
    if (e.key == " ")
	randomComic ();
}, false);
