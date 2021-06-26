import sys
from selenium import webdriver
from selenium.webdriver.common.keys import Keys

# get the height of a html page using selenium python

driver = webdriver.PhantomJS()
#driver.get("https://google.com")
fileName = sys.argv[1]

driver.get(fileName)
driver.maximize_window()
#driver.save_screenshot('testing.png')

#print(driver.execute_script("return document.body.scrollHeight"))
print(driver.execute_script("return document.body.offsetHeight"))