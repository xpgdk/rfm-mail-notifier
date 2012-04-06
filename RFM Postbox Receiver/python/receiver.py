#!/usr/bin/env python

import serial
import fdpexpect
import time
import sys
import smtplib
import string

def notify():
  SUBJECT = "Ny post"
  TO = "paul@xpg.dk"
  FROM = "postkassen@xpg.dk"
  text = "Post modtaget " + time.ctime()
  BODY = string.join((
    "From: %s" % FROM,
    "To: %s" % TO,
    "Subject: %s" % SUBJECT,
    "",
    text
    ), "\r\n")
  server = smtplib.SMTP("aspmx.l.google.com")
  server.sendmail(FROM, [TO], BODY)
  server.quit()

s = serial.Serial("/dev/ttyUSB0", 9600)

def readMsg(s):
  r = ""
  t = ""
  print "Readable:",s.readable()
  while t != "\n":
    t = s.read(1)
    print "R:",t
    r = r + t
  return r[:-1]

firstRun = True

while True:
  if firstRun:
    print "Initializing"

  s.write("I")

  resp = s.read(2)

  if resp != "O\n":
    print "Failed"
    sys.exit(1)

  if firstRun:
    print "Done"

  msg = s.readline()
  if len(msg) > 1:
    msg = msg[:-1]
    print time.ctime(),"-", msg
    if msg.startswith("SIGNAL:"):
      notify()

  time.sleep(2)
  firstRun = False
