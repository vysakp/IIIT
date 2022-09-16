function allowDrop(ev) {
    ev.preventDefault();
  }
  
  function drag(ev) {
    ev.dataTransfer.setData("text", ev.target.id);
  }
  
  function drop(ev) {
    ev.preventDefault();
    var data = ev.dataTransfer.getData("text");
    ev.target.appendChild(document.getElementById(data));
  }
  function Validateuser() {
          var user = document.getElementById("suname").value;
          var uError = document.getElementById("uError");
          uError.innerHTML = "";
          var expr = /(?=.*\d)(?=.*[A-Z])/;
          if (!expr.test(user)) {
              uError.innerHTML = "Invalid Username";
          }
      }
  function ValidateEmail() {
          var email = document.getElementById("email").value;
          var lblError = document.getElementById("lblError");
          lblError.innerHTML = "";
          var expr = /^([\w-\.]+)@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.)|(([\w-]+\.)+))([a-zA-Z]{2,4}|[0-9]{1,3})(\]?)$/;
          if (!expr.test(email)) {
              lblError.innerHTML = "Invalid email address.";
          }
      }
  var check = function() {
    if (document.getElementById('spass').value ==
      document.getElementById('cpass').value) {
      document.getElementById('message').style.color = 'green';
      document.getElementById('message').innerHTML = 'matching';
    } else {
      document.getElementById('message').style.color = 'red';
      document.getElementById('message').innerHTML = 'not matching';
    }
  }