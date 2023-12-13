const button = document.querySelector('button')

let req = {
    "Authorization": {
        "username": "zwoodson0",
        "password": "tehehhB5V<.&y(M"
    }
}

console.log(req)

onClick = async () => {
    let response = await fetch("http://127.0.0.1/auth", {
        method: "POST",
        body: JSON.stringify(req)
    })

    console.log(response.text());
}

button.addEventListener("click", onClick)