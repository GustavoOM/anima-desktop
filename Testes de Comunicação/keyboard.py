try:
    x = input()
    print ('Try using KeyboardInterrupt')
except KeyboardInterrupt:
    print ('KeyboardInterrupt exception is caught')
else:
    print ('No exceptions are caught')