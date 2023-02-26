#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME UserIsTaken

struct PLAYER_NAME : public Player
{

	/**
	 * Factory: returns a new instance of this class.
	 * Do not modify this function.
	 */
	static Player *factory()
	{
		return new PLAYER_NAME;
	}

	/**
	 * Types and attributes for your player can be defined here.
	 */

	const int NATURAL_DIRECTIONS_WIZARD = 4;
	const int NATURAL_DIRECTIONS_DWARF = 8;
	const int TRESHOLD = 80;
	const int BOARD_SIZE = 60;

#define NATURAL_POS (start_unit.type == UnitType::Dwarf) ? NATURAL_DIRECTIONS_DWARF : NATURAL_DIRECTIONS_WIZARD

/*debug purposes*/
#define HERE(str) std::cerr << str

	/*Return next direction to move following path betwen to positions*/
	Dir retrieve_dir(const std::map<Pos, Pos> &_vis, const Pos &fin, const Pos &start)
	{
		auto it = _vis.find(fin);
		while (it->second != start)
		{
			it = _vis.find(it->second);
		}
		Dir ret = Dir::None;
		if (it->first == (it->second + Dir::Bottom))
			ret = Dir::Bottom;
		if (it->first == (it->second + Dir::BR))
			ret = Dir::BR;
		if (it->first == (it->second + Dir::LB))
			ret = Dir::LB;
		if (it->first == (it->second + Dir::Left))
			ret = Dir::Left;
		if (it->first == (it->second + Dir::Right))
			ret = Dir::Right;
		if (it->first == (it->second + Dir::RT))
			ret = Dir::RT;
		if (it->first == (it->second + Dir::TL))
			ret = Dir::TL;
		if (it->first == (it->second + Dir::Top))
			ret = Dir::Top;
		return ret;
	}

	/*return valid adjacent position that is in valid path to target*/
	// beard in mind start_unit has the position we start from
	Dir path_to_target(const Unit &start_unit)
	{
		std::queue<Pos> next_on_line;
		std::map<Pos, Pos> visited_pos;

		Pos aux_val = start_unit.pos;
		Dir ret = Dir::None;
		bool found = false;

		visited_pos.insert(std::make_pair(aux_val, aux_val));
		next_on_line.push(aux_val);

		while (not next_on_line.empty() and not found)
		{
			aux_val = next_on_line.front();
			next_on_line.pop();

			/*Loop through all possible adjacent positions*/
			for (int i = 0; i < (NATURAL_POS); ++i)
			{
				/*we have 8 adjacents positions to move to for dwarves and 4 for the wizards*/
				/*possible directions to move to for wizards are enumerated as multiples of 2*/
				int param = (start_unit.type == UnitType::Dwarf) ? i : i * 2;
				Pos next_pos = Pos(aux_val) + Dir(param);

				// check wether the next position has been visited or not
				std::map<Pos, Pos>::const_iterator it = visited_pos.find(next_pos);
				bool pos_visited = it != visited_pos.end();

				if (pos_ok(next_pos) and not pos_visited)
				{
					/*we are inside the board and position has not been visited yet*/
					next_on_line.push(next_pos);
					/*mark position as visited*/
					visited_pos.insert(std::make_pair(next_pos, aux_val));
					Cell current_cell = cell(next_pos);
					Unit temp;
					/**
					 * - Avoid balrog
					 * - Avoid abysses
					 * - Go for enemi units
					 */
					/*Check wether there's a unit in the cell or not*/
					bool cell_has_unit = current_cell.id != -1;
					if (cell_has_unit)
						temp = unit(current_cell.id);
					bool is_my_unit = cell_has_unit and temp.player == me();

					if (start_unit.type == UnitType::Wizard)
					{
						/* Movement control for the wizard units */

						/* Wizards must avoid paths containing abysses, orcs, trolls or the balrog or units that arent mine */
						if (current_cell.type == CellType::Abyss)
							continue;
						if (current_cell.id == balrog_id())
							continue;
						if (cell_has_unit and temp.type == UnitType::Troll)
							continue;
						if (cell_has_unit and temp.type == UnitType::Orc)
							continue;
						/* Wizards are meant to move to my dwarves */
						if (is_my_unit and temp.type == UnitType::Dwarf)
						{
							ret = retrieve_dir(visited_pos, next_pos, start_unit.pos);

							/* if the adjacent position to go to has a dwarf dont move to it */
							Cell cell_temp = cell(Pos(start_unit.pos + ret));
							if (ret != Dir::None and cell_temp.id != -1 and unit(cell_temp.id).player == me())
							{
								ret = Dir::None;
							}
							found = true;
							break;
						}
					}
					else
					{
						/*do not enqueue the position if the balrog is there or there is an abys or a rock*/
						bool balrog = temp.id == balrog_id();
						bool abyss = current_cell.type == CellType::Abyss;

						/*Movement control for the dwarve units*/
						bool is_cave = current_cell.type == CellType::Cave;
						bool not_my_unit = temp.player >= 0 and temp.player <= 3 and temp.player != me();
						bool cell_is_mine = current_cell.owner == me();
						bool there_troll = cell_has_unit and temp.type == UnitType::Troll;
						bool already_conquered = not cell_is_mine and is_cave;
						bool not_conquered = current_cell.owner == -1 and is_cave;

						if (balrog or abyss or there_troll)
							continue;
						if (is_my_unit)
							continue;

						/*find enemy dwarf nearby to engage*/
						if (cell_has_unit and not_my_unit)
						{
							ret = retrieve_dir(visited_pos, next_pos, start_unit.pos);
							found = true;

							/*if we found and enemy we avoid looping*/
							/*thorugh the other adjacent positions*/
							break;
						}
						/*conquer nearby empty cell*/
						else if ((already_conquered or not_conquered))
						{
							ret = retrieve_dir(visited_pos, next_pos, start_unit.pos);
							found = true;
						}
					}
				}
			}
		}
		return ret;
	}

	/*move all wizards*/
	void move_wizards()
	{
		std::vector<int> my_wizards = wizards(me());
		Dir next_step;
		for (auto unit_id : my_wizards)
		{
			/*loop through all wizards for command*/
			Unit temp = unit(unit_id);
			next_step = path_to_target(temp);
			/*find for low health dwarf units if necessary*/
			command(unit_id, next_step);
		}
	}

	/*move all dwarves*/
	void move_dwarves()
	{
		std::vector<int> my_dwarves = dwarves(me());
		for (auto unit_id : my_dwarves)
		{
			/*loop through all dwarves for command*/
			Unit temp = unit(unit_id);
			Dir next_step = path_to_target(temp);
			command(unit_id, next_step);
		}
	}

	/**
	 * Play method, invoked once per each round.
	 */
	virtual void play()
	{
		move_wizards();
		move_dwarves();
	}
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
