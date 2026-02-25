from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.common.alert import Alert
import time


## selenium and geckodriver are required !!!

# Set up the Firefox driver (make sure geckodriver is installed and in your PATH)
driver = webdriver.Firefox()

# Open the local URL of the script runner 
driver.get("http://localhost:2900/tools/scriptrunner")  # Replace with the actual localhost URL

# Wait for a bit to see the result
time.sleep(2)

# find password textbox
password_input = driver.find_element(By.ID, "input-49")

# put the password
password_input.send_keys('nos3123!') 

#send the password
password_input.send_keys(Keys.RETURN)

# Wait for a bit to see the result
time.sleep(2)

#find text box 
input_script = driver.find_element(By.CLASS_NAME, "ace_text-input")

# script the orders you want to send
input_script.send_keys('cmd("ARDUCAM CAM_EXP3_CC")') 

# Wait for a bit to see the result
time.sleep(2)

# find "START" button
start_button = driver.find_element(By.CSS_SELECTOR, "button.v-btn--is-elevated:nth-child(1)   ")
      
# Click the button START
start_button.click()
    
# Wait for a bit to see the result
time.sleep(30)

# Close the browser after the task is completed
driver.quit()
