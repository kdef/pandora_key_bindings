var pauseBtn = document.getElementsByClassName('pauseButton')[0];
if (pauseBtn.style.display === 'block') {
    console.log('Pressing pause.');
    pauseBtn.click();
} else {
    console.log('Pressing play.');
    document.getElementsByClassName('playButton')[0].click();
}
