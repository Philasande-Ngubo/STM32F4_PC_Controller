import sys
import serial
import pc_com
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QTextEdit, QPushButton, QLabel, QGroupBox)
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QFont, QTextCursor, QPalette, QColor

class LCDDisplay(QTextEdit):
    def __init__(self):
        super().__init__()
        self.setMaximumHeight(80)
        self.setMinimumHeight(80)
        
        # LCD-like appearance
        font = QFont("Courier New", 16, QFont.Bold)
        self.setFont(font)
        
        # Dark background, green text (classic LCD look)
        palette = self.palette()
        palette.setColor(QPalette.Base, QColor(20, 60, 20))
        palette.setColor(QPalette.Text, QColor(0, 255, 0))
        self.setPalette(palette)
        
        # Set to read-only and remove scroll bars
        self.setReadOnly(False)
        self.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        
        # Initialize with empty lines
        self.setText("                \n                ")
        
    def get_text(self):
        """Get the current text from the LCD display"""
        return self.toPlainText()
    
    def set_text(self, text):
        """Set text ensuring 2 lines of 16 characters each"""
        lines = text.split('\n')
        
        # Ensure we have exactly 2 lines
        while len(lines) < 2:
            lines.append("")
        
        # Pad/truncate each line to 16 characters
        formatted_lines = []
        for line in lines[:2]:
            if len(line) > 16:
                formatted_lines.append(line[:16])
            else:
                formatted_lines.append(line.ljust(16))
        
        self.setText('\n'.join(formatted_lines))

    def keyPressEvent(self, event):
        """Override to enforce 2 lines of 16 characters"""
        super().keyPressEvent(event)
        
        # Get current text
        text = self.toPlainText()
        lines = text.split('\n')
        
        # Limit to 2 lines
        if len(lines) > 2:
            lines = lines[:2]
            self.setText('\n'.join(lines))
        
        # Check each line length
        modified = False
        for i, line in enumerate(lines):
            if len(line) > 16:
                lines[i] = line[:16]
                modified = True
        
        if modified:
            cursor_pos = self.textCursor().position()
            self.setText('\n'.join(lines))
            cursor = self.textCursor()
            cursor.setPosition(min(cursor_pos, len(self.toPlainText())))
            self.setTextCursor(cursor)


class ButtonIndicator(QWidget):
    def __init__(self, label):
        super().__init__()
        self.is_pressed = False
        self.label_text = label
        
        layout = QVBoxLayout()
        layout.setSpacing(5)
        
        # Create circular indicator
        self.indicator = QPushButton()
        self.indicator.setFixedSize(30, 30)
        self.indicator.setCheckable(True)
        self.indicator.toggled.connect(self.toggle_state)
        self.update_style()
        
        # Label
        label_widget = QLabel(label)
        label_widget.setAlignment(Qt.AlignCenter)
        
        layout.addWidget(self.indicator)
        layout.addWidget(label_widget)
        layout.setAlignment(Qt.AlignCenter)
        
        self.setLayout(layout)
    
    def toggle_state(self, checked):
        self.is_pressed = checked
        self.update_style()
    
    def update_style(self):
        if self.is_pressed:
            style = """
                QPushButton {
                    background-color: #FF0000;
                    border: 2px solid #AA0000;
                    border-radius: 15px;
                }
                QPushButton:hover {
                    background-color: #FF3333;
                }
            """
        else:
            style = """
                QPushButton {
                    background-color: #444444;
                    border: 2px solid #666666;
                    border-radius: 15px;
                }
                QPushButton:hover {
                    background-color: #555555;
                }
            """
        self.indicator.setStyleSheet(style)
    
    def get_state(self):
        return self.is_pressed
    
    def set_state(self, state):
        print(self.get_state);
        self.indicator.setChecked(state)


class STM32ControlUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()
        
    def initUI(self):
        self.setWindowTitle('STM32 LCD Control Interface')
        self.setGeometry(100, 100, 600, 400)
        
        # Central widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # Main layout
        main_layout = QVBoxLayout()
        main_layout.setSpacing(20)
        
        # Title
        title = QLabel('STM32F446 LCD Display')
        title.setFont(QFont('Arial', 16, QFont.Bold))
        title.setAlignment(Qt.AlignCenter)
        main_layout.addWidget(title)
        
        # LCD Display
        lcd_group = QGroupBox('LCD Display (2 Lines x 16 Characters)')
        lcd_layout = QVBoxLayout()
        self.lcd_display = LCDDisplay()
        lcd_layout.addWidget(self.lcd_display)
        lcd_group.setLayout(lcd_layout)
        main_layout.addWidget(lcd_group)
        
        # Button indicators
        button_group = QGroupBox('Button Status Indicators')
        button_layout = QHBoxLayout()
        button_layout.setSpacing(15)
        
        self.buttons = []
        for i in range(8):
            btn = ButtonIndicator(f'')
            self.buttons.append(btn)
            button_layout.addWidget(btn)
        
        button_group.setLayout(button_layout)
        main_layout.addWidget(button_group)
        
        # Control buttons
        control_layout = QHBoxLayout()
        
        self.send_button = QPushButton('Send State')
        self.send_button.setMinimumHeight(50)
        self.send_button.setFont(QFont('Arial', 12, QFont.Bold))
        self.send_button.setStyleSheet("""
            QPushButton {
                background-color: #4CAF50;
                color: white;
                border: none;
                border-radius: 5px;
            }
            QPushButton:hover {
                background-color: #45a049;
            }
            QPushButton:pressed {
                background-color: #3d8b40;
            }
        """)
        self.send_button.clicked.connect(self.send_state)
        
        self.clear_button = QPushButton('Clear All')
        self.clear_button.setMinimumHeight(50)
        self.clear_button.setFont(QFont('Arial', 12))
        self.clear_button.setStyleSheet("""
            QPushButton {
                background-color: #f44336;
                color: white;
                border: none;
                border-radius: 5px;
            }
            QPushButton:hover {
                background-color: #da190b;
            }
            QPushButton:pressed {
                background-color: #c41103;
            }
        """)
        self.clear_button.clicked.connect(self.clear_all)
        
        control_layout.addWidget(self.send_button, 3)
        control_layout.addWidget(self.clear_button, 1)
        
        main_layout.addLayout(control_layout)
        
        # Status label
        self.status_label = QLabel('Ready')
        self.status_label.setAlignment(Qt.AlignCenter)
        self.status_label.setStyleSheet("""
            QLabel {
                padding: 10px;
                background-color: #e0e0e0;
                border-radius: 5px;
            }
        """)
        main_layout.addWidget(self.status_label)
        
        central_widget.setLayout(main_layout)
        
        # Set window style
        self.setStyleSheet("""
            QMainWindow {
                background-color: #f5f5f5;
            }
            QGroupBox {
                font-weight: bold;
                border: 2px solid #cccccc;
                border-radius: 5px;
                margin-top: 10px;
                padding-top: 10px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px 0 5px;
            }
        """)
    
    def send_state(self):
        """Collect and display the current state"""
        lcd_text = self.lcd_display.get_text()
        button_states = [btn.get_state() for btn in self.buttons]
        
        # Format output
        status_text = f"LCD Text:\n"
        lines = lcd_text.split('\n')
        self.state_list = []

        self.state_list.append(lines[0])
        self.state_list.append(lines[1])
        
        LED = 0
        for i in range(len(button_states)):
            if (button_states[i]):
                LED += 2**(7-i)

        COMS = pc_com.controllers()
        if (COMS):
            BAUD_RATE = 115200
            ser = serial.Serial(COMS[0][0], BAUD_RATE, timeout=1)
            pc_com.send_and_receive(ser,"LI "+str(LED)+"\r\n");
            pc_com.send_and_receive(ser,"WR "+lines[0]+";"+lines[1]+"\r\n");
        
        # Update status label briefly
        self.status_label.setText('✓ State Sent Successfully')
        self.status_label.setStyleSheet("""
            QLabel {
                padding: 10px;
                background-color: #4CAF50;
                color: white;
                border-radius: 5px;
                font-weight: bold;
            }
        """)
        
        # Reset status after 2 seconds
        QTimer.singleShot(2000, self.reset_status)
    
    def clear_all(self):
        """Clear LCD display and reset all buttons"""
        self.lcd_display.set_text("")
        for btn in self.buttons:
            btn.set_state(False)
        self.status_label.setText('Cleared')
        QTimer.singleShot(1000, self.reset_status)
    
    def reset_status(self):
        self.status_label.setText('Ready')
        self.status_label.setStyleSheet("""
            QLabel {
                padding: 10px;
                background-color: #e0e0e0;
                border-radius: 5px;
            }
        """)


def main():
    app = QApplication(sys.argv)
    window = STM32ControlUI()
    window.setWindowTitle("No Maximize")
    window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()