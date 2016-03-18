void takeAction(int val) {
  String msg, subj;
  if (val == 3) {
    msg = "WARNING: CHILD LEFT IN CAR AT FATAL LEVELS! Stop what you are doing and"
    "return to your car now! The authorities have been notified.";
    subj = "CHILD IN DANGER, RETURN TO CAR";
    alertAuth(val);
  }
  else if (val == 2) {
    msg = "WARNING: Child left in car at dangerous levels! Stop what you are doing and"
    "return to your car now! The authorities have been notified.";
    subj = "Child in danger, return to car";
    alertAuth(val);
  }
  else { //warning, do not notify authorities right away
    msg = "WARNING: Child left in car and temperature levels rising. Stop what you are"
    "doing and return to your car before conditions become fatal!";
    subj = "Bask Alarm Warning";
  }
  
  TembooChoreo SendEmailChoreo;

  // Invoke the Temboo client
  SendEmailChoreo.begin();

  // Set Temboo account credentials
  SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
  SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);
  
  // Set Choreo inputs
  SendEmailChoreo.addInput("Username", "baskmalarm@gmail.com");
  SendEmailChoreo.addInput("Password", "ozpoluwutewtyeeq");
  SendEmailChoreo.addInput("Subject", subj);
  SendEmailChoreo.addInput("ToAddress", "b.li@columbia.edu");
  SendEmailChoreo.addInput("MessageBody", msg);
  
  // Identify the Choreo to run
  SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail");
  
  // Run the Choreo; when results are available, print them to serial
  SendEmailChoreo.run();
  
  while(SendEmailChoreo.available()) {
    char c = SendEmailChoreo.read();
    Serial.print(c);
  }
  SendEmailChoreo.close();
}

void alertAuth(int val) {
}

