const cComic = comicData.length >> 1;
const elImage = document.getElementById ("img");
const elButton = document.getElementById ("button");

function randomComic ()
{
    var nComic = Math.floor (Math.random() * cComic) << 1;
    elImage.src = comicData [nComic];
    elButton.innerText = comicData [nComic + 1];
}

elButton.addEventListener ("click", randomComic, false);
