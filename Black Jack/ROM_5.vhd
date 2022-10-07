library ieee;
use ieee.std_logic_1164.all;

entity ROM_5 is
  port ( A : in std_logic_vector(2 downto 0);
         Q : out std_logic_vector(7 downto 0));
end entity ROM_5;

architecture Behavioral of ROM_5 is
type M is array ( 0 to 7) of std_logic_vector(7 downto 0);
constant ROM : M := (	 0  => "00000000",
								 1  => "11100100",
								 2  => "11100110",
								 3  => "10100010",
								 4  => "10100010",
								 5  => "10111110",
								 6  => "10011100",
								 7  => "00000000");
								 
begin
	process (A)
	begin
		case A is
		when "000" => Q <= ROM(0);
		when "001" => Q <= ROM(1);
		when "010" => Q <= ROM(2);
		when "011" => Q <= ROM(3);
		when "100" => Q <= ROM(4);
		when "101" => Q <= ROM(5);
		when "110" => Q <= ROM(6);
		when "111" => Q <= ROM(7);
		end case;
	end process;

end architecture Behavioral;