class Object:
    def __init__(self):
        self.state = "OFF"

    def update_state(self, new_state):
        self.state = new_state
        return "200"
    
    def get_value(self):
        if self.state == "OFF":
            return "Object is OFF"

        # TODO implement function to get value from sensor node
        value = 10
        return value
    
    def get_consumption(self):
        if self.state == "OFF":
            return 0

        # TODO implement function to get the consumption of the node
        consumption = 10
        return consumption