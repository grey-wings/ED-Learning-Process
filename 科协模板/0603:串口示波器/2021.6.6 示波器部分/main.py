import sys
from PyQt5.QtWidgets import QApplication
import ser
import userGUI


app = QApplication(sys.argv)
win = userGUI.MyGraphWindows()
win.show()
sys.exit(app.exec_())
