void setup() {
  Serial1.begin(115200);

}

void loop() {
  Serial1.write("Work");
  delay(1000);

}
