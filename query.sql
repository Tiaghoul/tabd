SELECT  f.title, f.film_id, a.first_name, a.last_name
	FROM film f JOIN actor a JOIN film_actor ON film.film_id = film_actor.film_id AND actor.actor_id = film_actor.actor_id
	WHERE f.film_id = 1000 AND a.actor_id IN (SELECT actor.actor_id, COUNT(*) as C
		FROM film JOIN actor JOIN film_actor ON film.film_id = film_actor.film_id AND actor.actor_id = film_actor.actor_id
		GROUP BY actor.actor_id ORDER BY C desc
	)
	ORDER BY film.film_id;






OH YEAAAAH
SELECT actor.actor_id, first_name, last_name, COUNT(*) as C
	FROM film JOIN actor JOIN film_actor ON film.film_id = film_actor.film_id AND actor.actor_id = film_actor.actor_id
	WHERE actor.actor_id IN (SELECT  actor.actor_id
		FROM film JOIN actor  JOIN film_actor ON film.film_id = film_actor.film_id AND actor.actor_id = film_actor.actor_id
		WHERE film.film_id = 1000)
	GROUP BY actor.actor_id ORDER BY C desc
	LIMIT 1
