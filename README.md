# notepadzz
Minimalistic notes with markdown, keep-unsaved and toggleable UI  
Made with Qt5 in C++.  
  
![showcase](/github_images/showcase.png)

### (toggleable) features:  
markdown-like highlighting,  
monospace font,  
vertical tabs,  
horizontal tabs,  
folder viewer,  
zoom in/out

best usecase is when taking notes you hide all the ui,  
disable window decorations,  
and then you have just a floating textbox for taking notes:  
![mostMinimal](/github_images/minimal.png)
  
  
Ive used with >4k unsaved tabs (daily notes) and it works :D
`config.h` is where you set all shortcuts and save location/method.  
  
# Shortcuts
all editable within `config.h`  , as all other things are.  
  
`ctrl+N` - new tab   
`ctrl+H` - toggle markdown   
`ctrl+G` - toggle monospace font   
`ctrl+T` - toggle horizontal tabs  
`ctrl+Q` - toggle sidebar  
`ctrl+R` - rename current tab (warning: pushes it to bottom)  
`ctrl+PageUp` - next tab  
`ctrl+PageDown` - previous tab  
`ctrl+Plus` - zoom in  
`ctrl+Minus` - zoom out  
  
`ctrl+O` - open new file  
`ctrl+S` - save  
`ctrl+Shift+S` - save as  
  

# Prerequisites:  
  
`sudo apt install qtbase5-dev qt5-qmake`  
  
# Build:  
  
`make`  
  
or call `bash build.sh` which just calls make..  