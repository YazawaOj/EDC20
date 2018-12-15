from PIL import Image
import numpy as np
from PyQt5.QtWidgets import QMainWindow,QLabel,QApplication,QDesktopWidget,QLineEdit,QAction,QFileDialog
from PyQt5 import QtGui
from PyQt5.QtGui import QPalette,QColor
from PyQt5.QtCore import pyqtSignal
import sys
import re

def toHex(color):
    ans='#'
    for num in color:
        if(len(hex(num))==4):ans+=hex(num)[2:4]
        else:
            ans+='0'
            ans+=hex(num)[-1]
    return ans

def toColor(hex):
    if(len(hex)==7):
        r=int(hex[1:3].upper(),16)
        g=int(hex[3:5].upper(),16)
        b=int(hex[5:7].upper(),16)
        return [r,g,b]
    elif(len(hex)==9):
        r=int(hex[1:3].upper(),16)
        g=int(hex[3:5].upper(),16)
        b=int(hex[5:7].upper(),16)
        a=int(hex[7:9].upper(),16)
        return [r,g,b,a]
    else:
        return

#RGBA和十六进制颜色码的转换。六位颜色码为RGB，八位为RGBA

class MyLabel(QLabel):
    point=pyqtSignal(int,int)
    def __init__(self,parent=None):
        super().__init__(parent)
        self.setMouseTracking(True)

    def mouseMoveEvent(self,event):
        self.point.emit(event.x(),event.y())

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.labels=[]
        self.labelpic=QLabel(self)
        self.edits=[]
        self.color=[255,255,255,255]
        self.picturesize=[0,0]
        self.filename=''
        self.imglabel=MyLabel(self)
        self.img=None
        self.initUI()
        self.show()

    def initUI(self):
        '''
        初始化窗口和各部件
        '''
        labelR=QLabel('R',self)
        labelG=QLabel('G',self)
        labelB=QLabel('B',self)
        labelA=QLabel('A',self)
        labelH=QLabel('#',self)
        self.labels.append(labelR)
        self.labels.append(labelG)
        self.labels.append(labelB)
        self.labels.append(labelA)
        self.labels.append(labelH)
        for label in self.labels:
            label.hide()
        self.labelpic.hide()
        self.labelpic.setAutoFillBackground(True)
        self.imglabel.hide()
        self.imglabel.point.connect(self.showColor)

        editR=QLineEdit(str(self.color[0]),self)
        editG=QLineEdit(str(self.color[1]),self)
        editB=QLineEdit(str(self.color[2]),self)
        editA=QLineEdit(str(self.color[3]),self)
        editH=QLineEdit(toHex(self.color),self)
        self.edits.append(editR)
        self.edits.append(editG)
        self.edits.append(editB)
        self.edits.append(editA)
        self.edits.append(editH)
        for edit in self.edits:
            edit.textEdited.connect(self.changeColor)
            edit.hide()       

        act=QAction('Open',self)
        act.triggered.connect(self.openImg)
        act.setShortcut('Ctrl+P')
        menubar=self.menuBar()
        menu=menubar.addMenu('Menu')
        menu.addAction(act)

        self.resize(400,400)
        self.setWindowTitle('ColorViewer')
        self.center()

        #self.setLabels()
        #self.setEdits()

    def setLabels(self):
        '''
        根据picturesize修改各Label的位置
        '''
        for label in self.labels:
            label.show()
        for i in range(len(self.labels)):
            self.labels[i].setGeometry(self.picturesize[0]+80,240+i*30,20,20)
        self.labelpic.setGeometry(self.picturesize[0]+40,30,200,200)
        self.labelpic.show()

    def setEdits(self):
        '''
        根据picturesize修改各edit的位置
        '''
        for edit in self.edits:
            edit.show()
        for i in range(len(self.labels)):
            self.edits[i].setGeometry(self.picturesize[0]+100,240+i*30,100,20)

    def setTexts(self,x,y):
        '''
        根据color修改各lineedit的值
        '''
        #for i in range(4):
            #self.edits[i].setText(str(self.color[i]))
        #self.edits[4].setText(toHex(self.color))
        self.edits[0].setText(str(int(x/3)))
        self.edits[1].setText(str(int((810-y)/3)))

    def setColor(self):
        '''
        根据color修改labelpic上的颜色
        '''
        pe=QPalette()
        r,g,b,a=self.color
        pe.setColor(QPalette.Window,QColor(r,g,b,a))
        self.labelpic.setPalette(pe)

    def center(self):
        '''
        使窗口居中
        '''
        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())

    def openImg(self):
        self.filename=QFileDialog.getOpenFileName(self,'choose a image')[0]
        if(re.match(r'^.+\.(png|jpg|gif)$',self.filename)):
            image=Image.open(self.filename).convert('RGBA')
            pic=QtGui.QPixmap(self.filename)
            self.img=np.array(image)
            self.picturesize[0]=image.width
            self.picturesize[1]=image.height
            if image.height<360:
                self.resize(image.width+260,400)
            else:
                self.resize(image.width+260,image.height+40)
            self.center()
            self.imglabel.setGeometry(20,30,image.width,image.height)
            print(image.width,image.height)
            self.imglabel.setPixmap(pic)
            self.imglabel.show()
            self.setLabels()
            self.setEdits()
            self.setTexts(0,0)
        else:
            return

    def showColor(self,x,y):
        try:
            self.color=self.img[y][x]
            self.setTexts(x,y)
            self.setColor()
        except:
            return

    def changeColor(self):
        r,g,b,a,h=[edit.text() for edit in self.edits]
        try:
            r,g,b,a=int(r),int(g),int(b),int(a)
        except:
            return
        if(str([r,g,b,a])==str(self.color)):
            return
            self.color=toColor(h)
            if(not self.color): return
            if(len(self.color)==3): self.color.append(255)
            #self.setTexts()
            self.setColor()
        else:
            self.color=[r,g,b,a]
            #self.setTexts()
            self.setColor()


def main():
    app = QApplication(sys.argv)
    window=MainWindow()
    sys.exit(app.exec_())

if __name__=='__main__':
    main()

