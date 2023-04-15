import os

msg = input("Enter commit message: ")

os.system("git add .")
os.system("git commit -m" + msg)
os.system("git push origin main")