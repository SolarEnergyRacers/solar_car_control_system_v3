# Buhler Guest Login

## URLs

- [Register: guest.buhlergroup.com/guest](https://guest.buhlergroup.com/guest/register.php?switch_url=https://login.buhlergroup.com/login.html&ap_mac=00:2a:10:f4:ce:90&client_mac=b4:6b:fc:30:46:9d&wlan=guest&redirect=detectportal.firefox.com/canonical.html&_browser=1)
- [Login: guest.buhlergroup.com/guest/register_login_guest.php](https://guest.buhlergroup.com/guest/register_login_guest.php?_browser=1)

## Inputs

```html
<input type="email" style="width: 200px;" name="username" id="ID_formb58a091b_weblogin_username" value="" autocapitalize="none" autocorrect="off">

<input type="password" style="width: 200px;" name="password" id="ID_formb58a091b_weblogin_password" value="" autocomplete="new-password">

<input type="checkbox" id="ID_formb58a091b_weblogin_visitor_accept_terms" name="visitor_accept_terms" value="1">
```

## ID `form8a40ec07_weblogin` comes from

```html
<form name="form8a40ec07_weblogin" id="form8a40ec07_weblogin" 
      method="POST" action="/guest/register_login_guest.php?_browser=1" 
      accept-charset="UTF-8" enctype="application/x-www-form-urlencoded" 
      novalidate="novalidate">
```

## config.yml

```yaml
url: "https://guest.buhlergroup.com/guest/register_login_guest.php?browser=1"
username__Name: "username"
password__Name: "password"
accept_terms__Name: "visitor_accept_terms"
submit_button__Xpath: "//*[@id[contains(.,'weblogin_submit')]]"
wlan_name: "guest"
username: "ser@gemeinsamlernen.ch"
password: 018327
```

## Python Packages

pip install pyyaml

pip install selenium

## Scheduler

```bash
schtasks /create /tn "Guest relogin" /tr "python C:\Users\SER\guestLogin\captive_portal.py" /sc hourly /mo 6 /st 00:05
```

```bash
schtasks /create /tn "Guest login" /tr "python C:\Users\SER\guestLogin\captive_portal.py" /sc onstart
```



## TeamViewer

`226648383`

`t5wifcmj`

`yxzvssdn`

