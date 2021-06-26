import sys
from selenium import webdriver
from selenium.webdriver.common.keys import Keys

driver = webdriver.PhantomJS()
fileName = sys.argv[1]

driver.get(fileName)
driver.maximize_window()

print(driver.execute_script("return document.body.offsetHeight"))
