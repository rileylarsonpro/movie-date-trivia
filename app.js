// Require express and define app object
const express = require('express')
let app = express()
let cors = require('cors')

// Connect to mongodb
require('./mongo.js');
// Allow use of JSON and cors
app.use(express.json())
app.use(cors())


// Start up on port 8080 
const server = app.listen(8080, () => {
    console.log("Started server on port 8080")
})
// Import Movie model
const Movie = require('./models/movie.model');

function getRandomInt(max){
    return Math.floor(Math.random() * max);
}

function formatMovie(movie, difficulty){
    if(difficulty < 2){
        difficulty = 2
    }
    let question = {title: movie.Title, correct: movie.Year}
    let years = [null, null, null]
    let randPostion = getRandomInt(3)
    years[randPostion] = movie.Year
    years.forEach((year,index) => {
        let plusOrMinus = getRandomInt(2)
        if(year === null){
            let yearToAdd = 0
            if(plusOrMinus === 0) {
                yearToAdd = (movie.Year - getRandomInt(difficulty)) - 1
                // While answer is not unique 
                while(years.findIndex( year => year === yearToAdd) !== - 1){
                    yearToAdd = (movie.Year - getRandomInt(difficulty)) - 1
                }
                years[index] = yearToAdd
            }
            else {
                yearToAdd = (movie.Year + getRandomInt(difficulty)) + 1
                while((years.findIndex( year => year === yearToAdd) !== - 1) && yearToAdd < new Date().getFullYear()){
                    yearToAdd = (movie.Year + getRandomInt(difficulty)) + 1
                }
                years[index] = yearToAdd
            }
        }
    })
    years.forEach((year,index) => {
        question[`A${index + 1}`] = year
    })
    return question
}


app.get('/movie/:difficulty', cors(), (req,res) => {
    try {
        Movie.countDocuments({}, async function( err, count){
            var rand = getRandomInt(count)
            let movieToSend = await Movie.findOne().skip(rand)
            return res.status(200).send(formatMovie(movieToSend, req.params.difficulty))
        })
    } catch(err) {
        return res.status(500).send({message: "Internal Server Error", error: err});
    }
})

